#include "pdp8state.h"
#include <algorithm>



namespace PDP8 {
    void RegesterHistory::push(const Regesters& r) { hst[_pos] = r; _pos = (_pos+1) & HIST_MASK; _count = std::max(_pos+1,HIST_COUNT); }
    CpuState::CpuState() {
         power();
         m.resize(MAX_MEMORY);
         //for(int i=0;i<64;i++) _iots = nullptr;
    }

    void CpuState::power() {
        std::memset(&r,0,sizeof(Regesters));
        _sw = 0;
        _run = _singleStep=_interrupt_enable=_interrupt_enable_delay=_in_interrupt=_skip=false;
        _intrupet_request = 0;
        _state = State::Fetch;
    }
      Device::Device(CpuState& c) : c(c) {}
}



