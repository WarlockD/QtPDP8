#include "pdp8i.h"

static const int endian_int = 0x12345678;

/*
constexpr bool is_little_endian()
{
   // constexpr int a = ((int)0x12345678);
  //  constexpr unsigned char *c = (unsigned char*)(&endian_int);
   // if (*c == 0x78)
  //      return true;

    return *(unsigned char*)(&endian_int) == 0x78;
}

constexpr bool is_big_endian()
{
    constexpr int a = 0x12345678;
    constexpr unsigned char *c = (unsigned char*)(&a);
    if (*c == 0x12)
        return true;

    return false;
}
*/
 std::mutex main_thread_mtx;
namespace PDP8 {

int Emx8::GetTerminalOut() {
    if(debug_check)
        qDebug() << "We are already running?";
    debug_check = true;
    int ret = -1;
      state->mtx.lock();
      if(!terminal_out.empty()){
          ret = terminal_out.front();
          terminal_out.pop();
      }
      if(terminal_out.empty()) coutf = 0;
     state->mtx.unlock();
    debug_check = false;
    return ret;
}

// better or worst than a macro?
template<typename T> T static inline INC12(const T v) { return (v+1) & 07777; }
/*
 *
            md=mem[pc+ifl];					// FETCH
            ma=( ( md&0177 )+( md&0200?( pc&07600 ):0 ) )+ifl;
            pc=( ( pc+1 )&07777 );
            ac&=017777;
           total_time += memory_cycle_time;
            if ((md&07000)<06000)			// DEFER
                 total_time += memory_cycle_time;
                if ( md&0400 ) {
                    if ( ( ma&07770 )==010 ) mem[ma]++;
                    mem[ma]&=07777;
                    if (md&04000)
                        ma=mem[ma]+ifl;
                    else
                        ma=mem[ma]+dfl;
                }
                */

void checkint(PDP8_State& s) {
    if(s.checkInterrupt()) {
        s.ir = 4; // JMS
        s.ma = s.md = 0; // to 0
        s.state = State::Execute; // right to exicute
    } else {
        if(s.skip) { s.skip = false; s.pc = INC12(s.ma); } else s.pc = s.ma;
        s.state = State::Fetch; // right to exicute
    }
}


void fetch(PDP8_State& s) {
    s.pc = INC12(s.ma); // T1
     s.mb = s.mem[s.ma | (s.ifl << 12)]; // T2
    s.ir = s.mb & 07;
    if(s.interrupt_enable) s.interrupt_enable_delay = true;
    if(s.ir < 6) {
         s.ma=( ( s.mb&0177 )+( s.mb&0200?( s.pc&07600 ):0 ) ); // T4, zero or current page
          s.state = s.mb & 0100 ? State::Defer : s.ir == 5 ? State::Fetch : State::Execute; // next state
         if(s.ir == 5 && s.state == State::Execute) { // simple jump dosn't get out of fetch
             s.state = State::Fetch;
             checkint(s); // have to check int though
         }
    } else if(s.ir == 6) { // iot, filler right now, pass though a function table in PDP8 State
         if ( !(s.mb&0400) ) {
             if ( s.mb&0200 ) s.ac&=010000;
             if ( s.mb&0100 ) s.ac&=07777;
             if ( s.mb&040 ) s.ac^=07777;
             if ( s.mb&020 ) s.ac^=010000;
             if ( s.mb&1 ) s.ac = INC12(s.ac);
// missing shift stuff
         } else if(s.mb & 01) {
                 if((s.mb & 0x40) && ((s.ac & 0x800) > 0)) s.skip = true;
                 if((s.mb & 0x20) && ((s.ac & 0xFFF) == 0)) s.skip = true;
                 if((s.mb & 0x10) && ((s.ac & 0x1000) == 0)) s.skip = true;
                 if(s.mb&0x8) s.skip = !s.skip;
                 if(s.mb&0x80) s.ac &= 0x10000;
                 if(s.mb&0x4) s.ac |= s.sw & 07777;
                 if(s.mb&0x2) s.run = false;
             } else {
                 s.md = 0;
                 if((s.mb&0x80) || (s.mb & 0x10)) s.md = s.ac & 0xFFFF;
                 if(s.mb&0x40) s.md |= s.mq;
                 if((s.mb&0x80) || !(s.mb & 0x10)) s.mq = s.ac & 0xFFFF;
                 if(!(s.mb&0x80) || !(s.mb & 0x10)) s.mq = 0;
                 // eae here
                 s.ac = (s.ac & 0x10000) | s.md;
             }
             checkint(s);
         }
 }

void defer(PDP8_State& s) {
        s.mb = s.mem[s.ma];  //T1 md <- mem
        if ( ( s.ma&07770 )==010 ) // auto incrment
            s.mb = INC12(s.mb);

}
void execute(PDP8_State& s) {
    s.mb = s.mem[s.ma];  //T1 md <- mem
    // T2
    switch(s.ir & 07) {
        case 0: // AND
            s.ac &= s.mb;
        break;
    case 1: // TAND
            s.ac += s.mb;
    case 2: // ISZ
        // this should icrment during T1 though the gating network, but for now we just do this to save cycles
        s.mb = INC12(s.mb);
        s.skip = s.mb == 0;
        break;

    }
    if(s.skip) { s.skip = false; s.pc = INC12(s.ma); } else s.pc = s.ma;


    s.mem[s.ma] = s.mb; // T4,  might only use this latter
    if(!s.skip && s.checkInterrupt()) {

        s.ir = 0;
        s.state = State::Fetch;
    }
}
}
