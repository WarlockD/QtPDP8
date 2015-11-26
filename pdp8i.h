#ifndef PDP8I_H
#define PDP8I_H

#include "includes.h"
#include "pdp8interrupts.h"

#include "pdp8state.h"
#include "pdp8_utilities.h"
#include "pdp8dissam.h"
#include "timer.h"
#include "pdp8events.h"
namespace PDP8 {



    enum class  BreakState {
        Running,
        UserBreak,
        ButtonStop,
        ButtonPause,
        ProgramBreak
    };

    class Cpu : public CpuState {
        Bus _bus;
    public:


    protected:
        bool key_in;
        unsigned char key;
        int cinf;
        uint64_t time;
          int ilag;
        std::function<void(Cpu&)> _haltCallback;
        std::chrono::nanoseconds _current_cycle;
        RegesterHistory hst;
    public:
        Cpu() {

            cinf=ilag=0;
            _haltCallback =nullptr;
        }
        // carful, this is called from another thread
        void setHaltCallback(std::function<void(Cpu&)> c) { _haltCallback = c; }
        void installDev(DevicePtr ptr, char dev,bool defaultIntEnable) {
            installDevInt(dev,defaultIntEnable);
            //if(defaultIntEnable) _int._int_has_enable |=((uint64_t)1<<dev); else _int._int_has_enable &=~((uint64_t)1<<dev);
            _iots[dev & 077] = ptr;
        }
        void removeDevEnable(char dev) {
            removeDevEnableInt(dev);
            _iots[dev & 077] = nullptr;
        }


        void checkInterupts() {
            if(interruptPending()) {
                _int_ion = false;                                   /* interrupts off */
                 _no_ion_pending = false;
                 // so if ISZ 0 is called, and we get an interrupt, we get fucked
                 // so I put this here to "fix it"
                 // I will need to check the PDP8I manual so more to fix this issue
               //  if(_skip) { r.pc = (r.pc+ 1) & 07777; _skip = false; }
                r.sf = (r.UF << 6) | (r.IF >> 9) | (r.DF >> 12);    /* form save field */
                r.UF = r.IB = r.DF = r.UF = r.UB = 0;               /* clear mem ext */
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
            if(devcode==0) {
                switch(r.mb&07){
                case 0:                                     /* SKON */
                    if(_int_ion) setSkip();
                    _int_ion = false;
                      break;

                    case 1:                                     /* ION */
                     _int_ion = true;
                    _no_ion_pending = false;
                      break;

                    case 2:                                     /* IOF */
                       _int_ion = false;
                      break;

                    case 3:                                     /* SRQ */
                    if(_int_req !=0) setSkip();
                      break;

                    case 4:                                     /* GTF */
                    r.lac = (r.lac & 010000) | ((r.lac & 010000) >> 1) | (r.gtf << 10) | r.sf;
                    if(_int_req !=0) r.lac |= (1<<9);
                    if(_int_ion) r.lac |= (1<<7);
                      break;

                    case 5:                                     /* RTF */
                      r.gtf = ((r.lac & 02000) >> 10);
                      r.UB = (r.lac & 0100) >> 6;
                      r.IB = (r.lac & 0070) << 9;
                      r.DF = (r.lac & 0007) << 12;
                      r.lac = ((r.lac & 04000) << 1) | r.iot_data; // this is intresting, only in the pdp8e?
                      _int_ion = true;
                      _no_cif_pending = false;
                      break;

                    case 6:                                     /* SGT */
                      if (r.gtf)  setSkip();
                      break;

                    case 7:                                     /* CAF */
                      r.gtf = 0;
                      r.emode = 0;
                      _int_req = 0;
                      _int_ion = _no_ion_pending = false;
                        _no_cif_pending = false;
                      r.lac = 0;
                      for(int i=0;i<64;i++)
                          if(_iots[i]) _iots[i]->clear();/* reset all dev */
                      break;
                }

            } else {
                if(_iots[devcode]) _iots[devcode]->iot();
            }
        }

        int stepSwitch() {
        //  if (((this.data.fset == 1 ? 1 : 0) & (this.data.cpma == 191 ? 1 : 0)) != 0) {  this.data.run = true;
                  // if (this.data.intinprog) setInterruptOff;
            if( r.state == State::Fetch) r.ma = r.ifr << 12;
            switch( r.state) {
                case State::Fetch: fetch(); break;
                case State::Defer: defer(); break;
                case State::Execute: execute(); break;
            }
            return 0;
        }

        int32_t SetMa(int16_t md,int16_t ma){
           if ((md&07000)<06000)
            if ( md&0400 ) {
             if ( ( ma&07770 )==010 ) m[ma] = (m[ma]+1) &07777;
             return(m[ma]);
            }
            return ma;
          }


        void fetch() {
            _current_cycle += std::chrono::nanoseconds(15);
            _skip = false; // make sure skip is reset, ISZ before interrupt issue
             _no_ion_pending = true; // intrupets are
            // T1
             r.pc = (r.ma +1) & 0xFFF;
            // T2
             r.mb = m[r.ma | r.IF]; // T2
             r.ir = r.mb >> 9;
             r.opcode = r.mb;
             // T3
             switch(r.ir) {
             case 5:
                 if(!isIndirect(r.mb)) { // simple jump
                     r.pc=( ( r.mb&0177 )+( r.mb&0200?( r.pc&07600 ):0 ) );
                      _no_cif_pending = true;
                     break;
                 } // else we fall though to normal defer
             case 0: case 1: case 2: case 3: case 4:
                 r.ma=( ( r.mb&0177 )|( r.mb&0200?( r.ma&07600 ):0 ) );
                 r.state = isIndirect(r.mb) ? State::Defer : State::Execute ;
                 return;
             case 6:
                 iot();
                break;
             case 7:
                 if ( r.mb&0400 ) {
                    if ( r.mb&1 ) {group3( r.lac, r.mq, r.mb ); break; }
                    if ( r.mb&2 ) { _runningState = RunningState::Halt;  ; break; }
                    r.pc=group2( r.lac, r.pc, r.mb );
                    if ( r.mb&0200 ) r.lac&=010000;
                    if ( r.mb&4 )
                        r.lac|=_sw & 07777;
                    break;
                }
                r.lac=group1( r.lac, r.mb );
                break;
             }
             checkInterupts(); // T4
        }
        void defer() {
          _current_cycle += std::chrono::nanoseconds(15);
          if((r.ma & 07770) != 00010)
              r.mb = m[r.ma];
          else {
               _current_cycle += std::chrono::nanoseconds(2); //time += 2UL;  // check pdp8i specs
              r.mb = (m[r.ma] = (m[r.ma]+1) & 07777) ; // auto incrment
          }
          if(r.ir == 5) { // The flow says pc is set from mem/mem+1 but if its in b just move it
              r.pc = r.mb;
               _no_cif_pending = true;
                 r.state = State::Fetch;
              checkInterupts();
          } else {
              r.ma = r.mb;
               r.state = State::Execute;
          }

        }
              void execute() {
                  _current_cycle += std::chrono::nanoseconds(15);
                  r.mb = m[r.ma];
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
                      m[r.ma]=r.mb;
                   break;
                  case 3:
                      r.mb =r.lac&07777;
                      r.lac&=010000;
                      m[r.ma]=r.mb;
                   break;
                  case 4:
                       _no_cif_pending =  true;
                       if(_skip) { r.mb = (r.pc +1) &07777; _skip = false;} else r.mb = r.pc;
                      r.pc = (r.ma +1) & 07777;
                      m[r.ma]=r.mb;
                   break;
              }
                   r.state = State::Fetch;
                   checkInterupts();

              }
              void state_step(bool fullInstruction=false) {
                  //hst.push(r);
                do {
                      switch( r.state) {
                        case State::Fetch:
                          hst.push(r);
                          fetch(); break;
                        case State::Defer: defer(); break;
                        case State::Execute: execute(); break;
                      }
                  } while(fullInstruction && r.state != State::Fetch);

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

    int16_t group2(int16_t acc,int16_t xpc,int16_t xmd )
             {
                 int16_t state;

                 state=0;
                 if ( acc&04000 ) state|=0100;
                 if ( ( acc&07777 )==0 ) state|=040;
                 if ( acc&010000 ) state|=020;
                 if ( ( xmd&0160 )==0 ) state=0;
                 if ( xmd&010 ) {
                     if ( ( ~state&xmd )==xmd ) return( xpc+1 );
                     return( xpc );
                 }
                 if ( state&xmd ) return( xpc+1 );
                 return( xpc );
             }

     int16_t group1(int16_t acc,int16_t md )
             {
                 int16_t tmp;

                 if ( md&0200 ) acc&=010000;
                 if ( md&0100 ) acc&=07777;
                 if ( md&040 ) acc^=07777;
                 if ( md&020 ) acc^=010000;
                 if ( md&1 ) acc++;
                 acc&=017777;
                 if ( md&016 )
                     switch( md&016 )
                 {
                     case 2:  tmp=(acc<<6)|((acc>>6)&077);		// BSW .. v untidy!
                         tmp&=07777;
                         if (acc&010000) tmp|=010000;
                         acc=tmp;
                         break;
                     case 6:  acc=acc<<1; if ( acc&020000 ) acc++;
                     case 4:  acc=acc<<1; if ( acc&020000 ) acc++;
                         break;
                     case 10:  if ( acc&1 ) acc|=020000; acc=acc>>1;
                     case 8:  if ( acc&1 ) acc|=020000; acc=acc>>1;
                         break;
                 }
                 return( acc&017777 );
             }

    };


    class ThreadedCPU : public Cpu, public SimpleThread {
        RunningState _lastState;
        //std::mutex _mtx; // just in case
  public:
        ThreadedCPU();
    protected:
          // run this using std::thread
          bool threadFunction() override;
          void setRunningState(RunningState state) override;
    };

}


#endif // PDP8I_H
