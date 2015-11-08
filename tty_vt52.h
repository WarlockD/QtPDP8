#ifndef TTY_VT52_H
#define TTY_VT52_H

#include "includes.h"
#include "pdp8i.h"
// VT52 simple emulation for terminal
namespace PDP8 {
struct SerialInterface {
    virtual bool haveData() const =0;
    virtual int received()=0;
    virtual void trasmit(unsigned char data) =0;
    // If we have a screen
    virtual ~SerialInterface() {}
};
class VT52_Helper {
    uint16_t _curx;
    uint16_t _cury;
    uint16_t _width;
    uint16_t _height;
    uint8_t _last_char;
    bool _in_esc;
    char _in_direct_address;
    char _direct_buffer[2];

    std::string  _screen;



public:
    const std::string& getScreen() const { return _screen; }
    void setX(int v) { _curx = std::max(0,std::min(_width-1,v)); }
    void setY(int v) { _cury = std::max(0,std::min(_height-1,v)); }
    void setXY(int x, int y) { setX(x); setY(y); }
    void home() { _curx=_cury=0; }
    void retn() { _curx = 0; }

    VT52_Helper() : _curx(0),_cury(0),_last_char(0),_width(80), _height(24), _in_esc(false) ,_in_direct_address(0) {
        clearScreen();
    }
    void clearScreen() {
        _screen.resize(_width*_height);
        for(int i=0;i<(_width*_height);i++) _screen[i]=' ';
        _in_esc = false;
        _in_direct_address = 0;
    }
    void cursorUp(int n=1) { _cury = std::min(0,_cury-n); }
    void cursorDown(int n=1) { _cury = std::max(_height-1,_cury+n); }
    void cursorRight(int n=1) { _curx = std::max(0,_curx-n); }
    void cursorLeft(int n=1) { _curx = std::min(_height-1,_curx+n); }

    void homeCursor() { _curx = _cury = 0; } // upper left
    void linefeed() {
        if(_cury == 23) {
            std::copy(_screen.begin()+_width,_screen.end(),_screen.begin());
            std::fill(_screen.begin()+(_cury * _width),_screen.end(),' ');
        } else _cury++;
    }
    void revLinefeed() {
        if(_cury == 0) {
            std::copy(_screen.rbegin()+_width,_screen.rend(),_screen.rbegin());
            std::fill(_screen.begin(),_screen.begin()+ _width,' ');
        } else _cury--;
    }
    void putChar(char c) {
        _screen[(_cury * _width) + _curx] = c;
        if(_curx != (_width-1))_curx++;
    }

    void setX(uint16_t v) { _curx = std::min(v,(uint16_t)79); }
    void setY(uint16_t v) { _cury = std::min(v,(uint16_t)23); }
    void curDown() { if((_cury +1)<24)  _cury++; }
    void curUp() { if((_cury -1)>=0)  _cury--; }
    void curLeft() { if((_curx -1)>=0)  _curx--; }
    void curRight() { if((_curx +1)<80)  _curx++; }
    uint16_t x() const { return _curx; }
    uint16_t y() const { return _cury; }
    bool processChar(char c) {
        if(_in_direct_address!=0) {
            _direct_buffer[_in_direct_address-1] = c - 040;
            if(_in_direct_address==2) {
                setXY(_direct_buffer[1],_direct_buffer[0]);
                _in_direct_address = 0;
            }
        } else
        if(_in_esc) {
            switch(c) {
            case 'Z':
                _in_esc = false;
                return true;
                break;
            case 'B': // cursor down
                cursorDown();
                break;
            case 'I': // reverse line feed
                revLinefeed();
                break;
            case 'A':
                cursorUp();
                break;
            case 'C': // curser right
                cursorRight();
                break;
            case 0104: // backspace, what is 104?
                cursorLeft();
                break;
            case 'H': // home
                home();
                break;
            case 'Y':
                _in_direct_address = 1;
                break;
            default:
                qDebug() << "Bad Escepe Key: oct(" << QString::number(c,8) << ") " << QString::number(c);
                break;
            }
            _in_esc = false;
        } else {
            switch(c) {
           // case 016: // vt05 positiong
           //     _in_direct_address = 1;
           //     break; // flag this
            case 033: // esc
                _in_esc = true;
                break;
            case 040: // space
                putChar(c);
                break;
            case 010: // backspace
                cursorLeft();
                break;
            case 015: // return
                retn();
                break;
             case 011: // tab
                // mask hack
                setX((_curx +8) & 0x7);
                break;
            case 012: // linefeed
                linefeed();
                break;
            default:
                putChar(c);
                break;

            }
        }
        return false;
    }

};

class KL8C : public Device {
    static const uint32_t band110 = 100000;
    static const uint32_t baud150 = 66667;
    static const uint32_t baud300 = 33333;
    static const uint32_t baud600 = 16667;
    static const uint32_t baud1200 = 8333;
    static const uint32_t baud2400 = 4166;
    static const uint32_t baud4800 = 2083;
    static const uint32_t baud9600 = 1041;
    static const uint32_t baunds[];
    SimpleBuffer _outBuffer;
    SimpleBuffer _inBuffer;
    SerialInterface* _interface;
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
    void setInterface(SerialInterface* interface) { _interface = interface; }
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
        _interface = nullptr;
        _ttiEvent.setFunction([this]{
            if(_interface && _interface->haveData()) {
                _ttiData = _interface->received() | 0200;
                _ttiFlag = true;
                this->c.interruptRequest(_ttiDev,_intEnabled &&_ttiFlag);
                _ttiEvent.stop();
            }
        });
        //_ttiEvent.start();
         _ttoEvent.setFunction([this]{
             if(_interface) {
                 _interface->trasmit(_ttoData &0177);
                 _ttoFlag = true;
                 this->c.interruptRequest(_ttoDev,_intEnabled &&_ttoFlag);
             }
         });
    }


    void clear() override {
        while(_ttoEvent.running() && _ttiEvent.running()); // wait for events to finish
        _ttiFlag = _ttoFlag = false;
        _intEnabled = true;
    }
    void updateInterrupts() override {

    }

    void iot() override{
        Regesters& r = c.regs();
        switch( r.mb&0777 ) {
            case 030:  c.interruptRequest(_ttiDev,_intEnabled && (_ttiFlag=false));   break;
            case 031:  if(_ttiFlag) c.setSkip() ; break;
        case 032:  c.interruptRequest(_ttiDev,_intEnabled && (_ttiFlag=false));  r.lac &= 010000;_ttiEvent.start(); break;
        case 034:   r.lac |= _ttiData; break;
        case 035:  // not sure we should have this in the pdp8i
            _intEnabled = (r.lac & 1) !=0 ? true : false;
            c.interruptRequest(_ttiDev,_intEnabled && _ttiFlag);
            c.interruptRequest(_ttoDev,_intEnabled && _ttoFlag);
            break;
        case 036:
            c.interruptRequest(_ttiDev,_intEnabled && (_ttiFlag=false));
            r.lac &= 010000;
            r.lac |= _ttiData;
            _ttiEvent.start();
            break;
        case 040: c.interruptRequest(_ttoDev,_intEnabled && (_ttoFlag=true));  break;
        case 041: if(_ttoFlag) c.setSkip() ; break;
        case 042: c.interruptRequest(_ttoDev,_intEnabled && (_ttoFlag=false)); break;
        case 045:  if(_ttiFlag || _ttoFlag) c.setSkip(); break;
        case 046: c.interruptRequest(_ttoDev,_intEnabled && (_ttoFlag=false));
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
