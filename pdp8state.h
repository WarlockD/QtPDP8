#ifndef PDP8STATE_H
#define PDP8STATE_H

#include "includes.h"
#include "pdp8interrupts.h"
#include "pdp8dissam.h"
#include <array>
#include <set>

namespace PDP8 {
enum class State {
    Fetch,
    Defer,
    Execute,
    Iot,
    Opr
};
enum class RunningState {
     Halt=-1,   // when the cpu makes a halt op
    Stop=0,
    SingleStepInstruction,
    SingleStepState,
    Run,
    // These are debugging functions

};

struct Regesters {
    State state;
    int32_t pc;
    int32_t ea;
    int16_t ir;
    int16_t opcode;
    int16_t lac;
    int16_t mq;
    int16_t lfr;
    int16_t ma;
    int16_t mb;
    int16_t svr;
    int16_t gtf;
    int16_t sf;
    uint16_t iot_data; // humm garbage noise?
    char emode;
    char UF;
    char IF;
    char UB;
    char IB;
    char DF;
    char ibr;
    char ifr;
    char dfr;
};
class RegesterHistory {
    static const size_t HIST_COUNT = 64;
    static const size_t HIST_MASK = (HIST_COUNT-1);

    std::array<Regesters,HIST_COUNT> _array;//[HIST_COUNT];
    size_t _head;
    size_t _count;
    inline uint8_t increment(uint8_t v) const { return (v+1) & HIST_MASK; }
public:
    RegesterHistory() : _head(0) ,_count(0) {}
    bool push(const Regesters& r) {
        _array[_head] = r;
        _head = (_head+1) & HIST_MASK;
        if(_count < HIST_COUNT )_count++;
        return true;
    }
    const Regesters& last() const { return _array[(_head-1) & HIST_MASK]; }
    size_t count() const { return _count; }
    void clear() { _head = _count = 0; }
    // Did I mention I LOVE C++11 std::move?
    std::vector<const Regesters> dump() const;
    std::vector<const Regesters> dump(size_t count) const;
};

// this class is a serial interface to an internal of a device
// This is a byte interface that only lets you transver one byte at a time
// Its atomic though
// No need for atomics, but just to be safe, watch the order of setting the
// flags


typedef std::vector<uint16_t> MainMemory;

class CpuState;




class Device {
protected:
    CpuState& c;

    Device(CpuState& c);
public: // abstract interface
    virtual void tick() {}
    virtual void clear() = 0;
    virtual void iot() = 0;
    virtual void updateInterrupts()=0;
    virtual std::string debug() const = 0;
     virtual ~Device() {}
};

class Bus;
class BusEvent {
private: // EventQueue only has access to these
    Bus* _bus;
    uint64_t _delay; // internal delay
    BusEvent* _next;
public: // outside interface
    void addToBus(Bus* bus);
    void removeFromBus();
    bool start();
    bool stop();
    bool pause();
protected:
    uint64_t _interval;
    bool _restart;
    bool _isRunning; // only availabe in Bus
public:
    BusEvent(uint64_t  interval, bool restart=false) : _interval(interval), _restart(restart),_bus(nullptr) {}
    virtual ~BusEvent() {}
    bool restart() const { return _restart;}
    uint64_t interval() const { return _interval; }
    virtual bool tick() = 0; // return true to restart event
    friend class Bus;
};




typedef std::shared_ptr<Device> DevicePtr;

enum class PanelToggleSwitch {
    Start,
    Stop,
    Cont,
    SingleStepState,
    SingleStepInstruction,
    Dep,
    Exam,
    LoadAdd,
    Clear // Not a real switch. Does what start does without the run flipflop
};

class CpuState {

protected:

 std::atomic<RunningState> _runningState;
    static const size_t MAX_MEMORY = 32768;
    Regesters r;
    MainMemory m;
    uint32_t _sw;

    PanelToggleSwitch _panelSwitch; // used to figure out where the switch when we do a cont

    bool _no_ion_pending;
    bool _no_cif_pending;
    bool _int_ion;
                     /* intr enables */
    std::atomic<uint64_t>  _int_req ;                                      /* intr requests */
    uint64_t _int_has_enable; // mask for enables on reset

    bool _skip;
    // Devices and interrupts
    // After trial and error decided to just
    // copy the method used in simh verison of PDP8
    // The advantage is that we can check all the
    // dev's here and all the settings are convently
    // set up as a bunch of templates.
    DevicePtr _iots[64];
    friend class Device;
public:
    CpuState();

public: // diffrent switches
    // when the power switch is turned, this is turned on
    // clear eveything, like a power reset
    // carful, power clears EVEYTHING EXCEPT memory
    void power();

    inline DevicePtr findIot(size_t pos) { return _iots[pos]; }


    // when the start key is pressed
    inline void setSR(uint32_t i) { _sw = i; } // set the SR switch
    inline uint32_t getSR() const { return _sw; }
    void panelSwitch(PanelToggleSwitch s); // most are toggle so might not use state much


public: // getters and setters
    inline Regesters& regs() { return r; }
    inline unsigned short& operator[](int v) { return m[v]; }
    inline RunningState runningState() const { return _runningState.load(std::memory_order_seq_cst); }

    virtual void setRunningState(RunningState state) {
        _runningState.store(state,std::memory_order_seq_cst);
    }
    inline bool run() const { return _runningState == RunningState::Run; }
    inline bool singleStep() const { return _runningState == RunningState::SingleStepInstruction ||_runningState == RunningState::SingleStepState ; }
    inline bool skip() const { return _skip; }
    inline void setSkip() { _skip = true; }
    inline State state() const { return r.state; }
public:
    inline void enableInterrupts() { _int_ion = true; }
    inline  void disabbleInterrupts() { _int_ion = false; }

    void installDevInt(char dev,bool defaultIntEnable) {
        if(defaultIntEnable) _int_has_enable |=((uint64_t)1<<dev); else _int_has_enable &=~((uint64_t)1<<dev);
       // _iots[dev & 077] = ptr;
    }
    void removeDevEnableInt(char dev) {
        _int_has_enable &=~((uint64_t)1<<dev);
      //  _iots[dev & 077] = nullptr;
    }
    //inline void clearIonDelay() { _no_ion_pending = true; } // CPU uses this


  //  DevicePtr operator[](char dev) { return _iots[dev & 077]; }
    inline void setInterruptRequest(char dev) { _int_req |= ((uint64_t)1<<dev);  }
    inline void clearInterruptRequest(char dev) { _int_req &= ~((uint64_t)1<<dev);  }
    inline void interruptRequest(char dev, bool state) { if(state) _int_req |= ((uint64_t)1<<dev); else _int_req &= ~((uint64_t)1<<dev); }
    inline bool hasInterrupt(char dev) const { return (((uint64_t)1<<dev) & _int_req) != 0;  }
    inline bool interruptPending() const {
        if(_int_ion && _no_cif_pending && _no_ion_pending) return _int_req > 0;
        return false;
    }
    friend class Cpu; // frend of the CPU class since that calls can screw with anything
    friend class Emx8;
    friend class BasicInterruptSystem;
    std::string printState() const;
};

}



#endif // PDP8STATE_H
