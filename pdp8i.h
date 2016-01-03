#ifndef PDP8I_H
#define PDP8I_H

#include "includes.h"

#include "pdp8state.h"
#include "pdp8_utilities.h"
#include "pdp8dissam.h"
#include "timer.h"
#include "pdp8events.h"
namespace PDP8 {


    inline int16_t extend12bit(uint16_t a) { return a& 010000 ? (int16_t)(a & 0xF000) : (int16_t)(0x0FFF);  }

    enum class  BreakState {
        Running,
        UserBreak,
        ButtonStop,
        ButtonPause,
        ProgramBreak
    };



    class Cpu : public CpuState {


    protected:
        bool key_in;
        unsigned char key;
        int cinf;
        uint64_t time;
          int ilag;
        std::function<void(Cpu&)> _haltCallback;
        std::chrono::nanoseconds _current_cycle;

    public:
        Cpu() {
            _haltCallback =nullptr;

        }
        // carful, this is called from another thread
        void setHaltCallback(std::function<void(Cpu&)> c) { _haltCallback = c; }



        void checkInterupts() {
            if(interruptPending()) {
                _int_ion =_no_ion_pending = false;                                   /* interrupts off */
                 m.processInterrupt();
                r.ir = 04; // JMS
                r.ma = 0;
                r.state = State::Execute;
            } else {
                if(_skip) { r.ma = (r.pc+ 1) & 07777; _skip = false; } else  r.ma = r.pc;
                  r.state = State::Fetch;

            }

          }


        void iot() {
            int devcode = (r.mb & 0x1F8) >> 3;
           // int opcode = r.mb & 0x7;
           // if(_iots[devcode] != nullptr) _iots[devcode]->iot();
                switch(r.mb){
                /*
                case 0:                                     // SKON
                    if(_int_ion) setSkip();
                    _int_ion = false;
                      break;
*/
                    case 1:                                     //ION
                     _int_ion = true;
                    _no_ion_pending = false;
                      break;

                    case 2:                                     // IOF
                       _int_ion = false;
                      break;
/* // Humm.  Are none of these in the pdp8i specs?
                    case 3:                                     // SRQ
                    if(_int_req !=0) setSkip();
                      break;

                    case 4:                                     // GTF
                    r.lac = (r.lac & 010000) | ((r.lac & 010000) >> 1) | (r.gtf << 10) | r.sf;
                    if(_int_req !=0) r.lac |= (1<<9);
                    if(_int_ion) r.lac |= (1<<7);
                      break;

                    case 5:                                     // RTF
                      r.gtf = ((r.lac & 02000) >> 10);
                      r.ub = (r.lac & 0100) >> 6;
                      r.ib = (r.lac & 0070) << 9;
                      r.dfr = (r.lac & 0007) << 12;
                      r.lac = ((r.lac & 04000) << 1) | r.iot_data; // this is intresting, only in the pdp8e?
                      _int_ion = true;
                      _no_cif_pending = false;
                    break;

                    case 6:                                     // SGT
                      if (r.gtf)  setSkip();
                      break;

                    case 7:                                     // CAF
                      r.gtf = 0;
                      r.emode = 0;
                      _int_req = 0;
                        _int_ion = _no_ion_pending = false;
                        _no_cif_pending = false;
                      r.lac = 0;
                      for(int i=0;i<64;i++)
                          if(_iots[i]) _iots[i]->clear();// reset all dev
                      break;
                      */
                default:
                    if(r.mb & 00200) m.iot();
                    else
                    if(_iots[devcode]) _iots[devcode]->iot();
                }
        }

        int stepSwitch() {
        //  if (((this.data.fset == 1 ? 1 : 0) & (this.data.cpma == 191 ? 1 : 0)) != 0) {  this.data.run = true;
                  // if (this.data.intinprog) setInterruptOff;
           // if( r.state == State::Fetch) r.ma = r.ifr << 12;
            switch( r.state) {
                case State::Fetch: fetch(); break;
                case State::Defer: defer(); break;
                case State::Execute: execute(); break;
            }
            return 0;
        }



        void fetch() {
            _current_cycle += std::chrono::nanoseconds(15);
            _skip = false; // make sure skip is reset, ISZ before interrupt issue
             _no_ion_pending = true; // intrupets are
            // T1

             r.pc = (r.ma +1) & 0xFFF;
             m.pulseRead();  // T2

             // T3
             switch(r.ir) {
             case 5:
                 if(!isIndirect(r.mb)) { // simple jump
                     r.pc = r.mb&0177;
                     if(isCurrent(r.mb)) r.pc |= r.ma &07600;
                 } // else we fall though to normal defer
             case 0: case 1: case 2: case 3: case 4:
                 r.ma=( ( r.mb&0177 )|( r.mb&0200?( r.ma&07600 ):0 ) );
                 r.state = isIndirect(r.mb) ? State::Defer : State::Execute ;
                 return;
             case 6:
                 iot();
                break;
             case 7:
                 if(irb(8)) {
                     if(irb(0)) group3();
                     else group2();
                 } else group1();
                break;
             }
             checkInterupts(); // T4
        }
        void defer() {
          _current_cycle += std::chrono::nanoseconds(15);
          m.pulseRead();  // T2
          if((r.ma & 07770) == 00010){ // auto incrment
              _current_cycle += std::chrono::nanoseconds(2); //time += 2UL;  // check pdp8i specs
            //  r.mb = (r.mb +1) & 07777;
            // m[r.ma ] = r.mb;
          }
          if(r.ir == 5) { // The flow says pc is set from mem/mem+1 but if its in b just move it
              r.pc = r.mb;
              r.state = State::Fetch;
              checkInterupts();
          } else {
              r.ma = r.mb;
               r.state = State::Execute;
          }

        }
              void execute() {
                  _current_cycle += std::chrono::nanoseconds(15);
                  m.pulseRead();  // T2 have to run to set _ema
                  switch(r.ir) {
                  case 0:
                      r.lac&= r.mb |010000;
                    break;
                  case 1:
                      r.lac+=r.mb;
                      r.lac &=017777;
                   break;
                  case 2:
                      r.mb =( r.mb+1 )&07777;
                      if ( r.mb == 0 ) _skip = true;
                       m.pulseWrite();
                   break;
                  case 3:
                      r.mb =r.lac&07777;
                      r.lac&=010000;
                      m.pulseWrite();
                   break;
                  case 4:
                       if(_skip) { r.mb = (r.pc +1) &07777; _skip = false;} else r.mb = r.pc; // get rid of this maybe?
                      r.pc = (r.ma +1) & 07777;
                      m.pulseWrite();
                   break;
              }
                   r.state = State::Fetch;
                   checkInterupts();

              }
              void step() {
                  switch( r.state) {
                    case State::Fetch:   fetch(); break;
                    case State::Defer: defer(); break;
                    case State::Execute: execute(); break;
                  }
              }


              std::string printHistory(size_t i) ;
    private:
        void group3(int16_t& acc,int16_t& mmq,int16_t xmd )
            {
                int16_t qtm;

                if ( xmd&0200 ) acc=acc&010000;
                if ( xmd==07521 || xmd==07721 ) {
                    qtm=acc;
                    acc=mmq|( acc&010000 );
                    mmq=qtm&07777;
                    return;
                }
                if ( xmd&020 ) {mmq=acc&07777; acc&=010000; }
                if ( xmd&0100 ) acc|=mmq;
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
/*
        if(irb(7)) r.lac &= 010000;                                         // CLA
        // This is such a hack, but I just can't figure out how to do it otherwise
        if ( r.mb==07521 || r.mb==07721 ) {
            uint16_t tmp = r.lac &07777;
            r.lac |= r.mq;
            r.mq = tmp;
            return;
        }
        if(irb(4)) { r.mq = r.lac & 07777;  r.lac &= 010000; }  // MQL
        if(irb(6)) { r.lac |=  r.mq;   }              // MQA
        */
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
/*

        bool skip = !irb(3);
        if(irb(6) && accb(11)) skip = !skip;          // SMA/SPA
        if(irb(5) && ((r.lac&07777)==0)) skip = !skip;      // SNA/SZA
        if(irb(4) && accb(12)) skip = skip = !skip;             // SNL/SZL
       // if(irb(3)) skip = !skip;
        if(skip) setSkip();

        if(irb(7)) r.lac &= 010000;                     // CLA
        if(irb(2)) r.lac &= _sw & 07777;                // OSR
        if(irb(1)) _runningState = RunningState::Halt;  // HLT
        */
    }
#define HAVE_BSW
    void group1() {
        uint16_t lac = r.lac;
     //   if(irb(7)) r.lac &= 010000;                             // CLA
     //   if(irb(6)) r.lac &= 007777;                             // CLL
      //  if(irb(5)) r.lac ^= 007777;                             // CMA
     //   if(irb(4)) r.lac ^= 010000;                             // CML
     //   if(irb(0)) r.lac = (r.lac+1) & 017777;                  // IAC
        if(r.mb&0200) lac &= 010000;                             // CLA
        if(r.mb&0100) lac &= 007777;                             // CLL
       if(r.mb&040) lac ^= 007777;                             // CMA
        if(r.mb&020) lac ^= 010000;                             // CML
        if(r.mb&1) lac = (lac+1) & 017777;                  // IAC
            switch( r.mb&016 )
        {
            #ifdef HAVE_BSW
            case 2:
                //tmp=(acc<<6)|((acc>>6)&077);		// BSW .. v untidy!
               // tmp&=07777;
               // if (acc&010000) tmp|=010000;
               // acc=tmp;
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
    };

    //  class ThreadedCPU : public Cpu, public SimpleThread {
    class ThreadedCPU : public StupidPDP8, public SimpleThread {
        RunningState _lastState;
        // just a test but right now we run in 20 ms chucks and slow down for it
        // I like how this just works but I am sure I will need to
        // calabrate it somehow
        EventFunction _slowDownEvent;
        EventID _slowDownEventID;

        std::mutex _mutex;
        int _breakpoint;
  public:
        void setBreakPoint(int point) { _breakpoint = point; }
        ThreadedCPU();
        bool trylock(std::function<void(ThreadedCPU&)> func) {
            if(_mutex.try_lock()) {
                func(*this);
                _mutex.unlock();
                return true;
            }
            return false;
        }
        void waitLock(std::function<void(ThreadedCPU&)> func) {
            _mutex.lock();
            func(*this);
            _mutex.unlock();
        }
        void setRunningState(RunningState state) override;
    protected:

          // run this using std::thread
          bool threadFunction() override;
          void threadStopped()override;
    };

}


#endif // PDP8I_H
