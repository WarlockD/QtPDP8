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
    CpuState::CpuState() : _sw(0),  _run(false),_running(false),_singleStep(false),_int_has_enable(0){
         power();
         _int_has_enable=0;
         m.resize(MAX_MEMORY);
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
             r.state = State::Fetch;
            r.ma = r.pc;
            _int_ion = false;
            _no_cif_pending = _no_ion_pending = true;
            r.lac = 0;
            _singleStep = false;
            _run = true;
            break;
        case PanelToggleSwitch::Stop:
            _run = false;
            break;
        case PanelToggleSwitch::Cont:
            _run = true;
            break;
        case PanelToggleSwitch::Exam:
             r.state = State::Fetch;
            r.ma = r.pc;
            r.pc = (r.ma +1) & 07777;
            r.mb = m[r.ma];
            break;
        case PanelToggleSwitch::Dep:
             r.state = State::Fetch;
            r.ma = r.pc;
            r.pc = (r.ma +1) & 07777;
             m[r.ma] = r.mb;
            break;
        case PanelToggleSwitch::SingleStep:
            _run = false;
            _singleStep = true;
            break;
        case PanelToggleSwitch::LoadAdd:
            _run = false;
            r.ma = _sw & 07777;
            r.dfr = ((_sw>>12) & 07);
            r.ifr = ((_sw>>15) & 07);
            r.pc = r.ma;
            break;
        case PanelToggleSwitch::Clear :
        _int_ion = false;
        _no_cif_pending = _no_ion_pending = true;
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
        ss << "No Crf Pending : " << (_no_cif_pending ? "true" : "false") << std::endl;
        ss << "No ion Pending : " << (_no_ion_pending ? "true" : "false") << std::endl;
        ss << "Done           : "; formating_bits(_dev_done,ss); ss << std::endl;
        ss << "Enable         : "; formating_bits(_int_enable,ss); ss << std::endl;
        ss << "Req            : "; formating_bits(_int_req,ss); ss << std::endl;
        return ss.str();
    }
}



