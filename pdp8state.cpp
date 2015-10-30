#include "pdp8state.h"
#include <algorithm>



namespace PDP8 {
    void RegesterHistory::push(const Regesters& r) { hst[_pos] = r; _pos = (_pos+1) & HIST_MASK; _count = std::max(_pos+1,HIST_COUNT); }
    CpuState::CpuState() : _sw(0), _state(State::Fetch), _run(false),_running(false),_singleStep(false),_int_has_enable(0){
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
        _state = State::Fetch;
    }
    Device::Device(CpuState& c) : c(c) {}
    void CpuState::panelSwitch(PanelToggleSwitch s){// most are toggle so might not use state much
        switch(s) {
        case PanelToggleSwitch::Start:
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
            r.mb = m[r.ma=r.pc];
            r.pc = (r.pc+1) & 07777;
            break;
        case PanelToggleSwitch::Dep:
            r.mb = _sw & 07777;
            m[r.ma=r.pc] = r.mb;
            r.pc = (r.pc+1) & 07777;
            break;
        case PanelToggleSwitch::SingleStep:
            _run = false;
            _singleStep = true;
            break;
        case PanelToggleSwitch::LoadAdd:
            r.pc = _sw & 07777;
            r.dfr = ((_sw>>12) & 07);
            r.ifr = ((_sw>>15) & 07);
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
        ss << "Int Ion : " << (_int_ion ? "true" : "false") << std::endl;
        ss << "Done    : "; formating_bits(_dev_done,ss); ss << std::endl;
        ss << "Enable  : "; formating_bits(_int_enable,ss); ss << std::endl;
        ss << "Req     : "; formating_bits(_int_req,ss); ss << std::endl;
        return ss.str();
    }
}



