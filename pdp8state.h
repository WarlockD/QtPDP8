#ifndef PDP8STATE_H
#define PDP8STATE_H

#include "includes.h"
#include "pdp8interrupts.h"
#include "pdp8dissam.h"
#include "pdp8events.h"
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

#define bitmask(l) ((unsigned int)0xffffffff >> (31-(l)))
#define bbit(n)  ((mb & (1<<n)) ? true : false)
#define mb_bits(h,l)  ((mb >> l) & bitmask(h-l))

class DataBreak {
    bool _cycleSelect;
    bool _breakRequest;
    bool _dataDirection;
    bool _caIncrmented;
    bool _mbIncrmented;
    bool _transferDirection;
    uint16_t _address;
    uint16 _data;
};
// not sure this is needed?  Adder tests come up wierd
inline int16_t signExtend12bit(uint16_t v) { return (v & 04000) ? (int16_t)(0xF000 & v) : (int16_t)v; }

struct Regesters {
    const static size_t MAX_MEMORY =32768;
    const static size_t MAX_MEMORY_MASK =MAX_MEMORY-1;
    State state;
    uint16_t lac;

    int32_t pc;
    int32_t ema;  // extended address gatting
    int16_t ir;
    int16_t opcode;

    int16_t mq;

    int16_t ma;
    int16_t mb;
    int16_t svr;
    int16_t gtf;
    int16_t sf;
    uint16_t iot_data; // humm garbage noise?
    //extended memory
    int16_t ibr;
    int16_t ifr;
    int16_t dfr;
    int16_t lfr;
    int16_t uf;
    int16_t ub;
    uint16_t ib;
    char emode;
   // std::array<uint16_t,MAX_MEMORY> mem;
   // uint16_t& operator[](size_t i) { return mem[i & MAX_MEMORY_MASK]; }
   // uint16_t operator[](size_t i)const  { return mem[i & MAX_MEMORY_MASK]; }

};
class RegesterHistory {
    static const size_t HIST_COUNT = 64;
    static const size_t HIST_MASK = (HIST_COUNT-1);
    static inline size_t incAndRollOver(size_t v) { return  (v+1) & HIST_MASK; }
    static inline size_t decAndRollOver(size_t v) { return  (v-1) & HIST_MASK; }

    typedef std::array<Regesters,HIST_COUNT> HistroyArray;
    HistroyArray _array;//[HIST_COUNT];
    size_t _head;
    bool _overflow;
    inline uint8_t increment(uint8_t v) const { return (v+1) & HIST_MASK; }
    void incHead() {
        _head++;
        if(_head == HIST_COUNT) { _overflow = true; _head =0;}
    }
    size_t pos_begin() const { return _overflow ? incAndRollOver(_head)  : 0; }
    size_t pos_end() const { return  _head; } // head is always the end
    size_t rpos_begin() const { return _head != 0 ? decAndRollOver(_head) : 0;  } // _head-1 is always the first  valid element, if we got something
    size_t rpos_end() const { return _head; } // head is always the end }
public:
    class RegesterHistoryIterator: std::iterator<std::bidirectional_iterator_tag, Regesters > {
        const RegesterHistory* _hst;
        size_t _pos;
        bool _reverse;
        size_t next() { return (_pos = (_pos-1) & HIST_MASK);}
        size_t prev() { return (_pos = (_pos+1) & HIST_MASK);}
       public:
         RegesterHistoryIterator(const RegesterHistory* x, size_t startAt, bool reverse) :_hst(x),_pos(startAt),_reverse(reverse) {}
         RegesterHistoryIterator& operator++() { if(_reverse) next(); else prev(); return *this;}
         RegesterHistoryIterator operator++(int) { RegesterHistoryIterator tmp(*this); operator++(); return tmp;}
         RegesterHistoryIterator& operator--() { if(_reverse) prev(); else next();  return *this;}
         RegesterHistoryIterator operator--(int) { RegesterHistoryIterator tmp(*this); operator--(); return tmp;}
         bool operator==(const RegesterHistoryIterator& rhs)const {return _hst==rhs._hst && _pos == rhs._pos ;}
         bool operator!=(const RegesterHistoryIterator& rhs)const {return !(*this==rhs);}
         const Regesters& operator*() const {return _hst->_array[_pos]; ;}
         const Regesters* operator->() const {return &_hst->_array[_pos]; ;}
       };
    RegesterHistory() : _head(0) ,_overflow(false) {}
    bool push(const Regesters& r) {
        _array[_head] = r;
        incHead();
        return true;
    }
    RegesterHistoryIterator begin() const { return RegesterHistoryIterator(this,pos_begin(),false); }
    RegesterHistoryIterator end()const { return RegesterHistoryIterator(this,pos_end(),false); }
    RegesterHistoryIterator rbegin() const  { return RegesterHistoryIterator(this,rpos_begin(),true); }
    RegesterHistoryIterator rend() const  { return RegesterHistoryIterator(this,rpos_end(),true); }
    void clear() { _head = 0; _overflow = false; }
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
    virtual std::string debug() const { return std::string(); }
     virtual ~Device() {}
};




typedef std::shared_ptr<Device> DevicePtr;
// Cpustate, dev, function

//typedef std::function<void(CpuState&,char,char)> DeviceFunction;
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
public:
       EventSystem sim;
protected:
       static const size_t MAX_MEMORY = 32768;
       static const size_t USER_DEV = 01;
 std::atomic<RunningState> _runningState;

    Regesters r;

    uint32_t _sw;

    PanelToggleSwitch _panelSwitch; // used to figure out where the switch when we do a cont

    bool _no_ion_pending;
    bool _no_cif_pending;
    bool _int_ion;
    bool _user_interrupt;
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
protected:
    MainMemory m;
public: // memory interface
    inline MainMemory& mem() { return m; }
    uint16_t& operator[](size_t i) { return m[i]; } // throws exceptions
    uint16_t operator[](size_t i) const { return i < m.size() ?  m[i] : 0; }
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

    void installDevInt(int dev, DevicePtr ptr) {
        _iots[dev & 077] = ptr;
    }
    void removeDevEnableInt(int dev) {
        _iots[dev & 077] = nullptr;
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
    friend class PDP8_Silly;
    std::string printState() const;
};

}



#endif // PDP8STATE_H
