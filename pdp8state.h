#ifndef PDP8STATE_H
#define PDP8STATE_H

#include "includes.h"
#include "pdp8dissam.h"
#include "pdp8events.h"
#include <array>
#include <set>

namespace PDP8 {
enum class State {
    Fetch,
    Defer,
    Execute,
    Break,
    Iot,
    Opr
};
std::string to_octal(int num,const char* fmt="%04o") ;
inline bool isIndirect(uint16_t a) { return a & 0x100; }
inline bool isCurrent(uint16_t a) { return a & 0200; } //0200

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
    inline bool isIndirectOp() const { return isIndirect(opcode); }
    int16_t mq;

    int16_t ma;
    int16_t mb;
    int16_t gtf;
    uint16_t iot_data; // humm garbage noise?
    char emode;
    Regesters() {
         std::memset(this,0,sizeof(Regesters));
    }


};



class ExtendedMemory {
protected:
    Regesters& _r;
    uint8_t _ifr;
    uint8_t _ibr; // ib regester
    uint8_t _dfr; // Defer Regester
    uint8_t _bfr; // Break Regester
    uint8_t _sfr; // Save Flag Regester
    size_t _ema; // last address on bus
    bool _no_cif_inhibit;
    bool _indefer;
    std::array<uint16_t,4096*8> _mem;
    inline bool needdfr() const { return _r.isIndirectOp() && (_r.state == State::Defer || _r.state == State::Execute); }
    inline void fixAddr(size_t addr) { // preformance hit right here
         addr |= addr&07777;
         if(_r.ir == 04 || _r.ir == 05) {
             _no_cif_inhibit = true; // remove int inhibit
             _ifr = _ibr; // On JMP/JMS, move ib ot if
             addr |= _ifr << 9;
         } else {
             addr |= needdfr() ? _dfr << 9: _ifr << 9;
         }
         _ema = addr;
    }

public:
    void iot() {
        uint16_t op = _r.mb;
        if(op & 4){
            switch (op >> 3) {
            case 00: /* CINT */
                /* clear user interrupt flag */
                //_interrupt = false;
                //updateInterrupts();
                break;
            case 01: /* RDF */
                _r.lac |= _dfr;
                break;
            case 02: /* RIF */
                _r.lac |=  _ifr;
                break;
            case 03: /* RIB */
                _r.lac |=  _sfr;
                break;
            case 04: /* RMF */
                /* user state bit ? */
                _ibr = (_sfr & 070)>>3;
                _dfr = _sfr & 007;
               // r.ub = (r.sf & 0100) >> 6;
                _no_cif_inhibit = false; // disable interrupt till branch
                break;
            case 05: /* SINT */
                /* skip if user interrupt flag */
                // if (_userInterrupt) c.setSkip();
                break;
            case 06: /* CUF */
                /* clear user flag */
             //   r.ub = 0;
                break;
            case 07: /* SUF */
                /* set user flag, */
               // r.ub = 1;
                _no_cif_inhibit = false; // disable interrupt till branch
                break;
            }
            return;
        } else {
            if(op & 1)  _dfr = (op & 0070) >> 3;  // CDF
            if(op & 2)  { _ibr = (op & 0070) >> 3; _no_cif_inhibit = false;  }  // CIF
        }
    }
    ExtendedMemory(Regesters& r) : _r(r), _ifr(0),_dfr(0),_bfr(0),_ibr(0),_no_cif_inhibit(true),_indefer(false) {}
    void move_ib_to_if() { _ifr = _ibr; _no_cif_inhibit = true; }
    void processInterrupt() {
        _sfr = (_ibr <<3) | _dfr;
        _ifr=_ibr=_dfr=_bfr=0;
    }
    uint16_t* data() { return _mem.data(); }
    inline bool no_cif_inhibit() const { return _no_cif_inhibit; }
    inline uint8_t ifr() const { return _ifr; }
    inline uint8_t dfr() const { return _dfr; }
    inline uint8_t bfr() const { return _bfr; }
    inline uint8_t ibr() const { return _bfr; }
    inline size_t ema() const { return _ema; }
    inline void ibr(uint8_t v)  { _ifr=v; _no_cif_inhibit = false; }
    inline void ifr(uint8_t v)  { _ifr=v; }
    inline void dfr(uint8_t v)  { _dfr=v; }
    inline void bfr(uint8_t v)  { _bfr=v; }
    uint16_t sf() const { return _ifr | _dfr << 3; }
    void sf(uint16_t v) { _ifr = v & 3; _dfr = (v >> 3) & 3; }
    uint16_t absoluteRead(size_t addr) const { return _mem[addr]; }
    const uint16_t* data() const { return _mem.data(); }
    uint16_t& absoluteWrite(size_t addr) { return _mem[addr]; }
    inline uint16_t& operator[](size_t addr)   {
        //fixAddr(addr);
        return  _mem[addr];
    }
    void pulseRead() { // we set the regesters to what they are suppoed to be depending on the state
        switch(_r.state) {
            case State::Fetch:
                 _r.opcode = _r.mb = _mem[_ema= _r.ma | (_ifr << 9)]; // T2
                 _r.ir = _r.mb >> 9;
                 if(_r.ir == 05 &&  !isIndirect(_r.opcode)) move_ib_to_if(); // if simple jump makesure ibr is set
                 _indefer = false;
            break;
        case State::Defer:
            _r.mb = _mem[_ema=_r.ma | (_ifr << 9)]; // T2
            if ((_r.ma & 07770) == 00010)  _mem[_ema] = (_mem[_ema]+1) &07777; // auto inchrment
            if(_r.ir == 05) move_ib_to_if(); // defered jump for ibr
            _indefer = _r.ir < 04; // If we arn't a JMS or JMP
        break;
        case State::Execute:
             _r.mb = _mem[_ema=_r.ma | ((_indefer ? _dfr : _ifr) << 9)]; // T2
             if(_r.ir == 04) move_ib_to_if(); // defered jump for ibr
        break;
        }
    }
    void pulseWrite() {
        _mem[_ema] = _r.mb; // ema was already set in pulse read
    }
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
    RegesterHistory() : _head(0) ,_overflow(false) {
        std::memset(_array.data(),0,sizeof(Regesters)*HIST_COUNT);
    }
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
        RegesterHistory hst;
protected:
       static const size_t MAX_MEMORY = 32768;
       static const size_t USER_DEV = 01;
 RunningState _runningState;

    Regesters r;

    uint32_t _sw;

    PanelToggleSwitch _panelSwitch; // used to figure out where the switch when we do a cont

    bool _no_ion_pending;
    bool _no_cif_pending; // seperating the ema from the main cpu state
    bool _int_ion;
    bool _user_interrupt;
                     /* intr enables */
    uint64_t  _int_req ;                                      /* intr requests */
    uint64_t _int_has_enable; // mask for enables on reset

    bool _skip;
    // Devices and interrupts
    // After trial and error decided to just
    // copy the method used in simh verison of PDP8
    // The advantage is that we can check all the
    // dev's here and all the settings are convently
    // set up as a bunch of templates.
    std::array<Device*,64> _iots;
    friend class Device;
protected:
    ExtendedMemory m;

public:
    CpuState();
    uint16_t operator[](size_t i) const { return m.absoluteRead(i); }
    uint16_t& operator[](size_t i) { return m.absoluteWrite(i);}
    uint16_t* data() { return m.data(); }
public: // diffrent switches
    // when the power switch is turned, this is turned on
    // clear eveything, like a power reset
    // carful, power clears EVEYTHING EXCEPT memory
    void power();

    inline Device* findIot(size_t pos) { return _iots[pos]; }
    void installDev(Device* ptr, char dev) {
        _iots[dev & 077] = ptr;
    }
    void removeDevEnable(char dev) {
        _iots[dev & 077] = nullptr;
    }

    // when the start key is pressed
    inline void setSR(uint32_t i) { _sw = i; } // set the SR switch
    inline uint32_t getSR() const { return _sw; }
    void panelSwitch(PanelToggleSwitch s); // most are toggle so might not use state much


public: // getters and setters
    inline Regesters& regs() { return r; }
     inline RunningState runningState() const {
       //  return _runningState.load(std::memory_order_seq_cst);
          return _runningState;//.load(std::memory_order_seq_cst);
                                              }

    virtual void setRunningState(RunningState state) { _runningState = state;
       // _runningState.store(state,std::memory_order_seq_cst);
    }
    inline bool run() const { return _runningState == RunningState::Run; }
    inline bool singleStep() const { return _runningState == RunningState::SingleStepInstruction ||_runningState == RunningState::SingleStepState ; }
    inline bool skip() const { return _skip; }
    inline void setSkip() { _skip = true; }
    inline State state() const { return r.state; }
public:
    inline void enableInterrupts() { _int_ion = true; }
    inline  void disabbleInterrupts() { _int_ion = false; }

    void installDevInt(int dev, Device* ptr) {
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
        return _int_ion && _no_ion_pending&& _no_cif_pending && _int_req > 0;
    }
    friend class Cpu; // frend of the CPU class since that calls can screw with anything
    friend class Emx8;
    friend class BasicInterruptSystem;
    friend class PDP8_Silly;
    std::string printState() const;
    virtual void step() =0;
    void state_step(bool fullInstruction=false) {
        //hst.push(r);
      do {
            static const time_slice state_time(15); // 1.5us
            static const time_ms step_time(1000); // 1.5us
            if(singleStep())
                sim.incremtSimTime(step_time); // we just add a second to the single step
                else
            sim.incremtSimTime(state_time);
            sim.process_event();
            step();
        } while(fullInstruction && r.state != State::Fetch);

    }
};

#define bitmask(l) ((unsigned int)0xffffffff >> (31-(l)))
#define irb(n)  ((r.mb & (1<<n)) ? true : false)
#define accb(n)  ((r.lac & (1<<n)) ? true : false)

class StupidPDP8 : public CpuState{
   // std::array<uint16_t,4096*8> m;
    uint16_t ifr;
    uint16_t ifl;
    uint16_t dfr;
    uint16_t dfl;
    uint16_t svr;
    uint16_t usint;
    uint16_t uflag;
public:
    void clear(){
       ifr=ifl=dfr=dfl=svr=usint=uflag = 0;
   }
    StupidPDP8() { clear();

                 }
    void group3() {
        const bool CLA = irb(7);
        const bool MQA = irb(6);
        const bool MQL = irb(4); // Hopefuly the compiler gets rid of these
        uint16_t bus = 0;
        if(!CLA && !MQL) bus = r.lac & 07777;   // !CLA & !MQL
        if(MQA) bus |= r.mq;                // MQA
        if(MQL) if(CLA) r.mq = 0; else r.mq = r.lac & 07777;
        // EAE here
        r.lac = (r.lac & 010000) | bus;
    }

    void group2( ){ // from PDP8l? irb(3);

        uint8_t state =0;
        if((r.mb&0160)!=0) {
            if ((r.lac&004000) !=0 ) state|=0100;  // SMA/SPA
            if ((r.lac&007777 )==0 ) state|=040;   // SNA/SZA
            if ((r.lac&010000 )!=0 ) state|=020;   // SNL/SZL
        }
        // do we invert?
        if((r.mb&010)!=0) { if ( ( ~state&r.mb )==r.mb )  setSkip(); }
        else if ( state&r.mb  ) setSkip();

        if(irb(7)) r.lac &= 010000;                     // CLA
        if(irb(2))
            r.lac |= _sw & 07777;                // OSR
        if(irb(1)) _runningState = RunningState::Halt;  // HLT
   }
#define HAVE_BSW
    void group1() {
        uint16_t lac = r.lac;
        if(r.mb&0200) lac &= 010000;                             // CLA
        if(r.mb&0100) lac &= 007777;                             // CLL
       if(r.mb&040) lac ^= 007777;                             // CMA
        if(r.mb&020) lac ^= 010000;                             // CML
        if(r.mb&1) lac = (lac+1) & 017777;                  // IAC
            switch( r.mb&016 )
        {
            #ifdef HAVE_BSW
            case 2:
                 lac = (lac & 010000) & (((lac<<6)|((lac>>6)&077))&07777);
                break;
                #endif
            case 6:  lac=lac<<1; if ( lac&020000 ) lac|=1;
            case 4:  lac=lac<<1; if ( lac&020000 ) lac|=1;
                break;
            case 10:  if ( lac&1 ) lac|=020000; lac=lac>>1;
            case 8:  if ( lac&1 ) lac|=020000; lac=lac>>1;
                break;
        }
            r.lac = lac;

    }


    void step() {
        uint16_t* m = data();

        if(interruptPending()) {
            m[0]=r.pc;
            r.pc=1;
            _int_ion = _no_ion_pending = false;
            svr=(ifl>>9)+(dfl>>12);
            if(uflag==0) svr |= 0100;
            dfr=ifr=dfl=ifl=uflag=0;
        }
     // if (_int_ion) _no_ion_pending = true;
        r.mb=m[r.pc+ifl];					// FETCH
        r.ir = r.mb >> 6;
        r.ma=( ( r.mb&0177 )+( r.mb&0200?( r.pc&07600 ):0 ) )+ifl;
        r.pc=( ( r.pc+1 )&07777 );

        // diag
        r.opcode = r.mb;

       // r.lac&=017777;
        if ((r.mb&07000)<06000)	{		// DEFER
            if ( r.mb&0400 ) {
                if ( ( r.ma&07770 )==010 ){ m[r.ma]++; m[r.ma]&=07777; }
                if (r.mb&04000)
                    r.ma=m[r.ma]+ifl;
                else
                    r.ma=m[r.ma]+dfl;
            }
        }
        if(r.mb > 07777) throw std::runtime_error("Bad MB");
        if(r.ma > 017777) throw std::runtime_error("Bad MA");
        switch ( r.mb&07000 )			// EXECUTE
        {
        case 0000:
            r.lac&=(m[r.ma]|010000 );
            break;
        case 01000:
            r.lac+=m[r.ma];
            break;
        case 02000:
            if ((m[r.ma] = (m[r.ma]+1)&07777) == 0 ) setSkip();
            break;
        case 03000:
            m[r.ma]=r.lac&07777;
            r.lac&=010000;
            break;
        case 04000:
            ifl=ifr<<9;
            r.ma=(r.ma&07777)+ifl;
            m[r.ma]=r.pc;
            r.pc=( r.ma+1 )&07777;
            _no_cif_pending = true;
            uflag|=2;
            break;
        case 05000:
            r.pc= (r.ma&07777);
            ifl=ifr<<9;
            _no_cif_pending = true;
            uflag|=2;
            break;
        case 06000: // bare bones here
        {
            switch(r.mb) {
            case 06001://ION
                _int_ion = true;
               _no_ion_pending = false;
                break;
            case 06002: // IOF
                  _int_ion = false;
                break;
            default:
                int devcode = (r.mb & 0x1F8) >> 3;
                if(_iots[devcode]) _iots[devcode]->iot();
                break;
            }
        }
        break;

        case 07000:
            if(irb(8)) {
                if(irb(0)) group3();
                else group2();
            } else group1();
           break;
        }
             if (_int_ion && r.mb!=06001) _no_ion_pending = true;
       if(_skip) { r.pc=( ( r.pc+1 )&07777 ); _skip = false; }
    }

};

}



#endif // PDP8STATE_H
