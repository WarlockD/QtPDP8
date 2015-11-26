#include "pdp8i.h"
#include <iomanip>

static const int endian_int = 0x12345678;
static std::mutex pdp8cpu_mutex; // locking this might take preformance hits
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
    void formatListLine(std::stringstream& ss,const Dissasembler::Line& l,bool endofline=true) {
        ss << octzero(l.addr) << ' ' << octzero(l.data) << ' ';
        ss << std::left << std::setw(20)<< std::setfill(' ') << l.line;

        if(!l.comment.empty()){
            ss << '/';
            ss << std::left << std::setw(30)<< std::setfill(' ') << l.comment;
        }


        if(endofline) ss << std::endl;
    }

    std::string Cpu::printHistory(size_t count)  {
             std::stringstream ss;
             Dissasembler dsam(&m[0]);
             if(count == 1) {
                 const Regesters& r = hst.last();
                 const Dissasembler::Line& line = dsam.atAddr(r.pc);
                 formatListLine(ss,line);
             } else {
                 auto dump = hst.dump(count);
                 for(const Regesters& r : dump) {
                     const Dissasembler::Line& l = dsam.atAddr(r.pc);
                     formatListLine(ss,l);
                 }
             }
             return std::move(ss.str()); // dos
          }

    ThreadedCPU::ThreadedCPU() {
        startThread();
    }
    void ThreadedCPU::setRunningState(RunningState state)  {
        Cpu::setRunningState(state);
    }
    using namespace std::chrono;
    bool ThreadedCPU::threadFunction()  {
        RunningState c_runningState = runningState();
        switch(c_runningState) {
            case RunningState::Run:
        {
            RunningState _lastState = c_runningState;
            _current_cycle =  std::chrono::nanoseconds::zero();//  0;
            auto start = std::chrono::high_resolution_clock::now();
            for(int i=0;i<1000;i++) {
                state_step();
                // this might be to hevey
                if(_lastState != runningState()) break;
            }
            auto end = std::chrono::high_resolution_clock::now();
           // std::chrono::duration<double> diff = end-start;
            auto mdiff = duration_cast<nanoseconds>(end-start);
            if(mdiff < _current_cycle)
                std::this_thread::sleep_for(mdiff - _current_cycle);
            if(_haltCallback && runningState() == RunningState::Halt) _haltCallback(*this);
        }
            break;
        case RunningState::Halt:
        case RunningState::Stop:
          //  waitFor([&_runningState](){ return _runningState != RunningState::Stop && _runningState != RunningState::Halt; });
            std::this_thread::sleep_for(milliseconds(100));
            break;
        case RunningState::SingleStepInstruction:
            state_step(true);
            _lastState = RunningState::Stop;
            break;
        case RunningState::SingleStepState:
            state_step(false);
            _lastState = RunningState::Stop;
            break;
        default: // ugh
            _lastState = RunningState::Stop;
            break;
        }
      return true; // always return true as this never stops
  }
}
