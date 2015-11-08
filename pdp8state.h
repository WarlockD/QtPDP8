#ifndef PDP8STATE_H
#define PDP8STATE_H

#include "includes.h"
#include "pdp8interrupts.h"
#include <array>

namespace PDP8 {
enum class State {
    Fetch,
    Defer,
    Execute,
    Iot,
    Opr
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

    std::array<Regesters,HIST_COUNT> hst;//[HIST_COUNT];
    size_t _head;
    size_t _tail;
    size_t _count;
public:
    RegesterHistory() : _tail(0), _head(0) ,_count(0) {
      std::memset(&hst,0,sizeof(Regesters)*64);
    }
    void push(const Regesters& r) {
        if(_count == HIST_COUNT){
            _tail = _head;
        }
        hst[_head] = r;
        _head = (_head + 1) % HIST_COUNT;
        _count = std::max(_count+1,HIST_COUNT);
    }
    size_t count() const { return _count; }
    size_t begin() const { return _head-1; }
    size_t end() const { return _tail; }
    const Regesters& operator[](size_t i) const { return hst[i% HIST_COUNT]; }

    std::string printDisam(size_t count) const;
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
    virtual void clear() = 0;
    virtual void iot() = 0;
    virtual void updateInterrupts()=0;
    virtual std::string debug() const = 0;
     virtual ~Device() {}

};
typedef std::shared_ptr<Device> DevicePtr;

enum class PanelToggleSwitch {
    Start,
    Stop,
    Cont,
    SingleStep,
    Dep,
    Exam,
    LoadAdd,
    Clear // Not a real switch. Does what start does without the run flipflop
};

class CpuState {
protected:
    static const size_t MAX_MEMORY = 32768;
    Regesters r;
    MainMemory m;
    uint32_t _sw;

    bool _run;      // the state of the engine
    bool _running; // on if the thread is running
    bool _singleStep;
    bool _no_ion_pending;
    bool _no_cif_pending;
    bool _int_ion;

    std::atomic<uint64_t> _dev_done;// = 0;                                     /* dev done flags */
    std::atomic<uint64_t>  _int_enable;// = INT_INIT_ENABLE;                     /* intr enables */
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
    inline bool run() const { return _run; }
    inline bool running() const { return _running; }
    inline bool singleStep() const { return _singleStep; }
    inline bool skip() const { return _skip; }
    inline void setSkip() { _skip = true; }
    inline State state() const { return r.state; }
public:
    inline void enableInterrupts() { _int_ion = true; }
    inline  void disabbleInterrupts() { _int_ion = false; }
    inline void updateInterrupts()  { _int_req = (_int_req &~_int_has_enable) | (_dev_done & _int_enable); }

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
    inline void enableDevInterrupt(char dev) {  _int_enable |= ((uint64_t)1<<dev); }
    inline void disableDevInterrupt(char dev) { _int_enable &= ~((uint64_t)1<<dev); }
    inline bool isInterruptEnabled(char dev) const { return ((_int_has_enable & ((uint64_t)1<<dev)) & _int_enable) != 0; }
    inline void setInterruptRequest(char dev) { _int_req |= ((uint64_t)1<<dev);  }
    inline void clearInterruptRequest(char dev) { _int_req &= ~((uint64_t)1<<dev);  }
    inline void interruptRequest(char dev, bool state) { if(state) _int_req |= ((uint64_t)1<<dev); else _int_req &= ~((uint64_t)1<<dev); }
    inline bool hasInterrupt(char dev) const { return ((_int_enable & ((uint64_t)1<<dev)) & _int_req) != 0;  }
    inline bool isDone(char dev) const { return _dev_done & ((uint64_t)1<<dev); }
    inline void setDone(char dev)  { _dev_done |= ((uint64_t)1<<dev);  }
    inline void clearDone(char dev)  { _dev_done &= ~((uint64_t)1<<dev); }
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
