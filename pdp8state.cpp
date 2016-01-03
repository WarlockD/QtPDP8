#include "pdp8state.h"
#include <algorithm>


inline size_t next_number(size_t current,size_t size) { return (current + 1) % size; }

namespace PDP8 {
/*
    bool RegesterHistory::push(const Regesters& r) {
        size_t nextHead = next_number(_head,HIST_COUNT);
        _count = std::max((size_t)(_pos+1),(size_t)HIST_COUNT);
        if(nextHead == _tail) _tail = _head;
        _head = nextHead;
        return true;
    }
    bool RegesterHistory::pop(Regesters& r) {
        if(_count == 0) return false;
        r = hst[_tail];
        _tail = next_number(_tail,HIST_COUNT);
        _count = std::min(0,_count-1);
        return true;
    }

*/
    CpuState::CpuState() : m(r), _sw(0), _runningState(RunningState::Stop){
        memset(_iots.data(),0,sizeof(Device*)*64);

         power();
         _int_has_enable=0;
         //m.resize(MAX_MEMORY);
         /*
                 _sw = 0;
                 uint32_t _sw;
                 State _state;

                 bool _run;      // the state of the engine
                 bool _running; // on if the thread is running
                 bool _singleStep;
                 bool _no_ion_pending;
                 bool _no_cif_pending;
                 bool _int_ion;

                 std::atomic<uint64_t> _dev_done;// = 0;
                 std::atomic<uint64_t>  _int_enable;// = INT_INIT_ENABLE;
                 std::atomic<uint64_t>  _int_req ;
                 uint64_t _int_has_enable; // mask for enables on reset
                 */
    }

    void CpuState::power() {
        std::memset(&r,0,sizeof(Regesters));
        _int_ion = false;
        _no_cif_pending = _no_ion_pending = true;
        _int_req = 0;
         r.state = State::Fetch;
    }
    Device::Device(CpuState& c) : c(c) {}
    void CpuState::panelSwitch(PanelToggleSwitch s){// most are toggle so might not use state much

        switch(s) {
        case PanelToggleSwitch::Start:
              _panelSwitch = s;
             r.state = State::Fetch;
            r.ma = r.pc;
            _int_ion = false;
            _no_ion_pending = true;
            r.lac = 0;
            setRunningState(RunningState::Run);
            break;
        case PanelToggleSwitch::SingleStepInstruction:
        case PanelToggleSwitch::SingleStepState:
              _panelSwitch = s;
        case PanelToggleSwitch::Stop:
            setRunningState(RunningState::Stop);
            break;
        case PanelToggleSwitch::Cont:
            switch(_panelSwitch) {
            case PanelToggleSwitch::SingleStepInstruction: // single instruction
                setRunningState(RunningState::SingleStepInstruction);
                break;
            case PanelToggleSwitch::SingleStepState: // single instruction
                setRunningState(RunningState::SingleStepState);
                break;
            default:
                setRunningState(RunningState::Run);// any other mode we just run
            }
            break;
        case PanelToggleSwitch::Exam:
            setRunningState(RunningState::Stop);
             r.state = State::Fetch;
            r.ma = r.pc;
            r.pc = (r.ma +1) & 07777;
            r.mb = m[r.ma];
            break;
        case PanelToggleSwitch::Dep:
            setRunningState(RunningState::Stop);
             r.state = State::Fetch;
            r.ma = r.pc;
            r.pc = (r.ma +1) & 07777;
             m[r.ma] = r.mb;
            break;
        case PanelToggleSwitch::LoadAdd:
            setRunningState(RunningState::Stop);
            r.ma = _sw & 07777;
            //r.dfr = ((_sw>>12) & 07);
          //  r.ifr = ((_sw>>15) & 07);
            r.pc = r.ma;
            break;
        case PanelToggleSwitch::Clear:
            setRunningState(RunningState::Stop);
            _int_ion = false;
            _no_ion_pending = true;
            r.lac = 0;
        break;
        }
    }
    void formating_bits(uint64_t v, std::stringstream& ss) {
        for(int i=0;i<64;i++,v>>=1) {
            if(v & 1) ss << std::oct << i << " ";
        }
    }




    std::string CpuState::printState() const {
        std::stringstream ss;
        ss << "Int Ion        : " << (_int_ion ? "true" : "false") << std::endl;
        //ss << "No Crf Pending : " << (_no_cif_pending ? "true" : "false") << std::endl;
        ss << "No ion Pending : " << (_no_ion_pending ? "true" : "false") << std::endl;

        ss << "Req            : "; formating_bits(_int_req,ss); ss << std::endl;
        return ss.str();
    }


}



