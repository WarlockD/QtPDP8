#ifndef TTY_VT52_H
#define TTY_VT52_H

#include "includes.h"
#include "pdp8i.h"
#include "terminal.h"
#include "pdp8events.h"
// VT52 simple emulation for terminal
namespace PDP8 {
enum class KL8CBand :uint32_t{
    Band110 = 100000,
    Baud150 = 66667,
    Baud300 = 33333,
    Baud600 = 16667,
    Baud1200 = 8333,
    Baud2400 = 4166,
    Baud4800 = 2083,
    Baud9600 = 1041,
};

class KL8C : public Device, public Terminal::SerialDCEInterface {

    static const uint32_t baunds[];//= { KL8C::band110,KL8C::baud150,KL8C::baud300,KL8C::baud600,KL8C::baud1200,KL8C::baud2400,KL8C::baud4800,KL8C::baud9600 };
    SimpleBuffer _outBuffer;
    SimpleBuffer _inBuffer;
    char _ttiDev;
    char _ttoDev;
    unsigned char _ttiData;
    unsigned char _ttoData;
    bool _ttiFlag;
    bool _ttoFlag;
    bool _intEnabled;
    KL8CBand _currentBand;
    time_slice _band; //(100);
    EventFunction _ttiEvent;
    EventFunction _ttoEvent;
public:

    KL8C(CpuState& c) :   Device(c) {
        _ttiDev = 03;
        _ttoDev = 04;
        setBaud(KL8CBand::Band110);
        _ttiEvent = [this]() {
                     if(internalRecive(_ttiData)) {
                         _ttiData |= 0200;
                         _ttiFlag = true;
                         updateInterrupts();
                     }
                     return _band;
                 };
        _ttoEvent = [this]() {
            internalTrasmit(_ttoData &0177); // lock this thread till we have free space to send?
           // while(!_trasmitBuffer.push(_ttoData &0177));
            _ttoFlag = true;
            updateInterrupts();
            return time_slice::zero();
        };

        enableTTI();
    }
    void enableTTI() {
        c.sim.activate(&_ttiEvent,_band);
    }
    void enableTTO() {
        c.sim.activate(&_ttoEvent,_band);
    }
    void setBaud(KL8CBand b) {
        _currentBand = b;
        clear();
    }



    void clear() override {
        c.sim.cancel(&_ttiEvent);
        c.sim.cancel(&_ttoEvent);
        _ttiFlag = _ttoFlag = false;
        _intEnabled = true;
    }
    void updateInterrupts() override {
        c.interruptRequest(_ttiDev,_intEnabled && _ttiFlag);
        c.interruptRequest(_ttoDev,_intEnabled && _ttoFlag);
    }

    void iot() override{
        Regesters& r = c.regs();
        switch( r.mb&0777 ) {
            case 030:  _ttiFlag=false; updateInterrupts();  break;
            case 031:
            if(_ttiFlag) c.setSkip() ; break;
        case 032:  _ttiFlag=false; updateInterrupts();  r.lac &= 010000; break;
        case 034:   r.lac |= _ttiData; break;
        case 035:  // not sure we should have this in the pdp8i
            _intEnabled = (r.lac & 1) !=0 ? true : false;
            updateInterrupts();
            break;
        case 036:
            _ttiFlag=false; updateInterrupts();
            r.lac &= 010000;
            r.lac |= _ttiData;
            enableTTI();
            break;
        case 040: _ttoFlag=true;  updateInterrupts(); break;
        case 041: if(_ttoFlag) c.setSkip() ; break;
        case 042: _ttoFlag=false;  updateInterrupts(); break;
        case 045:  if(_ttiFlag || _ttoFlag) c.setSkip(); break;
        case 046: _ttoFlag=false;  updateInterrupts();
                // fall though
        case 044:  _ttoData=r.lac&0377; enableTTO(); break;
      }
    }
    std::string debug() const  override {
        std::stringstream s;
        s << " _intEnabled=" << (_intEnabled ? "true" : "false");
        s << " _ttiFlag=" << (_ttiFlag ? "true" : "false");
        s << " _ttoFlag=" << (_ttoFlag ? "true" : "false");
        s << std::endl;
        return s.str();
    }
};

}


#endif // TTY_VT52_H
