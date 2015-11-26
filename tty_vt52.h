#ifndef TTY_VT52_H
#define TTY_VT52_H

#include "includes.h"
#include "pdp8i.h"
#include "terminal.h"
// VT52 simple emulation for terminal
namespace PDP8 {

class KL8C : public Device, public Terminal::SerialDCEInterface {
    static const uint32_t band110 = 100000;
    static const uint32_t baud150 = 66667;
    static const uint32_t baud300 = 33333;
    static const uint32_t baud600 = 16667;
    static const uint32_t baud1200 = 8333;
    static const uint32_t baud2400 = 4166;
    static const uint32_t baud4800 = 2083;
    static const uint32_t baud9600 = 1041;
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
    char _currentBand;
    SimpleTimer _ttiEvent; // runs all the time
    SimpleEvent _ttoEvent;


public:
    void setBaud(char v) {
        _currentBand = v & 7;
        _ttiEvent.setDelay(std::chrono::microseconds(baunds[_currentBand]));
        _ttoEvent.setDelay(std::chrono::microseconds(baunds[_currentBand]));
        clear();

    }

    KL8C(CpuState& c) :   Device(c) {
        setBaud(0);
        _ttiDev = 03;
        _ttoDev = 04;
        _ttiEvent.setFunction([this]{
             if(internalRecive(_ttiData)) {
                 _ttiData |= 0200;
                 _ttiFlag = true;
                 updateInterrupts();
             }
        });
        //_ttiEvent.start();
         _ttoEvent.setFunction([this]{
             internalTrasmit(_ttoData &0177); // lock this thread till we have free space to send?
            // while(!_trasmitBuffer.push(_ttoData &0177));
             _ttoFlag = true;
             updateInterrupts();
         });
         _ttiEvent.start();
    }


    void clear() override {
        while(_ttoEvent.running() && _ttiEvent.running()); // wait for events to finish
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
        case 032:  _ttiFlag=false; updateInterrupts();  r.lac &= 010000;_ttiEvent.start(); break;
        case 034:   r.lac |= _ttiData; break;
        case 035:  // not sure we should have this in the pdp8i
            _intEnabled = (r.lac & 1) !=0 ? true : false;
            updateInterrupts();
            break;
        case 036:
            _ttiFlag=false; updateInterrupts();
            r.lac &= 010000;
            r.lac |= _ttiData;
            _ttiEvent.start();
            break;
        case 040: _ttoFlag=true;  updateInterrupts(); break;
        case 041: if(_ttoFlag) c.setSkip() ; break;
        case 042: _ttoFlag=false;  updateInterrupts(); break;
        case 045:  if(_ttiFlag || _ttoFlag) c.setSkip(); break;
        case 046: _ttoFlag=false;  updateInterrupts();
                // fall though
        case 044:  _ttoData=r.lac&0377; _ttoEvent.start(); break;
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
