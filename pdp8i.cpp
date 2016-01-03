#include "pdp8i.h"
#include <iomanip>
void test_debug(const QString& msg);
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
                 const Regesters& r = *hst.rend();
                 const Dissasembler::Line& line = dsam.atAddr(r.pc);
                 formatListLine(ss,line);
             } else {
                // auto dump = hst.dump(count);
                 for(const Regesters& r : hst) {
                     const Dissasembler::Line& l = dsam.atAddr(r.pc);
                     formatListLine(ss,l);
                 }
             }
             return std::move(ss.str()); // dos
          }

    ThreadedCPU::ThreadedCPU() {
        _breakpoint = -1;
        _slowDownEvent = [this]() {
       // std::this_thread::sleep_for(time_ms(20)); // fix this
                return time_ms(20);
    };
        sim.activate(&_slowDownEvent,time_ms(20));
   }
    void ThreadedCPU::setRunningState(RunningState state)  {
        RunningState c_runningState = runningState();
        if(c_runningState == state) return; // nothing to do
        if(c_runningState == RunningState::Run) { state = RunningState::Stop; stopThread();}// gotto stop the thread first in all situations
        switch(state) {
            case RunningState::Run:
             CpuState::setRunningState(state);

              hst.clear();
            startThread();
            return;
        case RunningState::Halt:
        case RunningState::Stop:
            state = RunningState::Stop; // not sure why Halt would come up, convert it to stop
            break;
        case RunningState::SingleStepInstruction:
        {
            CpuState::setRunningState(RunningState::SingleStepInstruction);
            std::stringstream s;
            s << "LAC=" << ostr(r.lac,5) << " MA=" << ostr(r.ma,5) << " MB=" << ostr(r.mb,4) << " PC=" << ostr(r.pc,5) << " : " << dsam8(m[r.pc],r.pc,&m[0],true);
            qDebug() << QString::fromStdString(s.str());
            state_step(true);
            state = RunningState::Stop;
        }
            break;
        case RunningState::SingleStepState:
        {
             CpuState::setRunningState(RunningState::SingleStepState);
            std::stringstream s;
             s << "LAC=" << ostr(r.lac,5) << " MA=" << ostr(r.ma,5) << " MB=" << ostr(r.mb,4) << " PC=" << ostr(r.pc,5) << " : " << dsam8(m[r.pc],r.pc,&m[0],true);
            qDebug() << QString::fromStdString(s.str());
            state_step(false);
            state = RunningState::Stop;
        }
            break;
        default: // ugh
            CpuState::setRunningState(RunningState::Stop);
            break;
        }
        CpuState::setRunningState(state);
    }
    using namespace std::chrono;

    bool ThreadedCPU::threadFunction()  {
        bool keep_running = true;
        RunningState c_runningState = runningState();
        RunningState _lastState = c_runningState;
     //   _current_cycle =  std::chrono::nanoseconds::zero();//  0;
     //   auto start = std::chrono::high_resolution_clock::now();
        pdp8cpu_mutex.lock();

        for(int i=0;i<19763;i++) {

             hst.push(r);
            state_step();
            // this might be to hevey
            if(_lastState != runningState()) {
                keep_running = false;
                break;
            }
            if(_breakpoint > -1 && _breakpoint == r.pc) {
                keep_running = false;
                break;
            }
        }
        pdp8cpu_mutex.unlock();
        return keep_running; // always run
       }

    void ThreadedCPU::threadStopped() {
        std::stringstream s;
        RegesterHistory::RegesterHistoryIterator start= hst.begin();
        RegesterHistory::RegesterHistoryIterator end= hst.end();
        int count=0;

        for(auto it = start ;it != end && count < 16;it++,count++) {
            const Regesters& regs = *it;
            s << "LAC=" << ostr(regs.lac,5) << " MQ=" << ostr(regs.mq,5) << " PC=" << ostr(regs.pc,5) << " : " << dsam8(m[regs.ma | regs.ema],regs.ma,&m[0],true) << std::endl;
        }
        s << "NEXT-------------\n";
         qDebug() << s.str().c_str();
         s.clear();
        count=0;
        start = hst.rbegin();
        end = hst.rend();
        for(auto it = start ;it != end && count < 16;it++,count++) {
            const Regesters& regs = *it;
            s << "LAC=" << ostr(regs.lac,5) << " MQ=" << ostr(regs.mq,5) << " PC=" << ostr(regs.pc,5) << " : " << dsam8(m[regs.ma | regs.ema],regs.ma,&m[0],true) << std::endl;
        }
       // static QTextStream ts( stdout );
      //  test_debug(QString::fromStdString(s.str()));
       qDebug() << s.str().c_str();
    }
}
