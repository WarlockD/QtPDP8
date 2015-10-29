#include "pdp8state.h"
#include <algorithm>



namespace PDP8 {
    void RegesterHistory::push(const Regesters& r) { hst[_pos] = r; _pos = (_pos+1) & HIST_MASK; _count = std::max(_pos+1,HIST_COUNT); }
    CpuState::CpuState() {
         power();
         m.resize(MAX_MEMORY);
                 _sw = 0;
    }

    void CpuState::power() {
        std::memset(&r,0,sizeof(Regesters));

        _run = _singleStep=_interrupt_enable=_interrupt_enable_delay=_in_interrupt=_skip=false;
        _intrupet_request = 0;
        _state = State::Fetch;
    }
    Device::Device(CpuState& c) : c(c) {}
    void CpuState::panelSwitch(PanelToggleSwitch s){// most are toggle so might not use state much
        switch(s) {
        case PanelToggleSwitch::Start:
            _interrupt_enable = false;
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
        }
    }

}



