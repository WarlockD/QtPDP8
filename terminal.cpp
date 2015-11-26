#include "terminal.h"
#define RETURN 015
#define LINEFEED 012
#define BACKSPACE 010
#define TAB 011
#define SPACE 040
#define DELETE 0177
#define ESC 033
#define XOFF 023
#define XON 021
namespace Terminal {

class CharRingQueue {
    static const size_t SIZE = 0x100;
    static const unsigned char MASK = 0xFF;
    char _ring[SIZE];
    std::atomic_uint8_t  _head;
    std::atomic_uint8_t  _tail;
public:
    CharRingQueue() : _head(0), _tail(0) {}
    bool pop(char& c) {
        if(_tail  < _head) {
            c = _ring[_tail++ & MASK];
            return true;
        }
        return false; // no pop
    }
    bool push(char c) {
        if((_tail + SIZE) > _head){
            c = _ring[_head++ & MASK];
            return true;
        }
        return false;// full no push
    }
};


/*
class SerialPortInterface {
    const uint8_t XOFF = 0x13;
    const uint8_t XON = 0x11;
public:
    enum class FlowControl {
        Hardware,
        Software,
        None
    };
protected:
    std::atomic_uint32_t _band;
    FlowControl _flowControl;
    bool _isDTE;
    bool _isFullDuplex;
    FIFOBuffer _trasmitBuffer;
    FIFOBuffer _recivedBuffer;

    std::function<void()> _onRecivedData;
    std::atomic_flag _running;
    std::shared_ptr<SerialPortInterface> _recivingPort;
public:
*/

class NoAttribute : public Attribute {
public:
 NoAttribute() {}
 bool underline() const { return false; }
 bool bold() const { return false; }
 bool blink() const { return false; }
 bool inverted() const { return false; }
 bool altChar() const { return false; }
 bool doubleheight() const { return false; }
 bool doublewidth() const { return false; }
 size_t foreground() const { return false; }
 size_t background() const { return false; }
  static const NoAttribute noAttribute;
};
const NoAttribute NoAttribute::noAttribute;
class SpaceCharBitmap  : public CharBitmap {
 static const unsigned char _data[];
public:
 SpaceCharBitmap() {}
 const unsigned char* scanline(size_t y) const { return _data + y*scanlineLength(); }
 const unsigned char* data() const { return _data; }
 size_t height() const { return 10; }
 size_t width() const { return 10; }
 int charCode() const { return ' '; }
 const Attribute& attribute() const {
     return NoAttribute::noAttribute;
 }
 static const SpaceCharBitmap spaceCharBitmap; // filler
};
const unsigned char SpaceCharBitmap::_data[] = {
 0,0,0,0,0,0,0,0,0,0  /* to make it 32 byte alinged, add two bytes */ ,0,0,
 0,0,0,0,0,0,0,0,0,0  /* to make it 32 byte alinged, add two bytes */ ,0,0,
 0,0,0,0,0,0,0,0,0,0  /* to make it 32 byte alinged, add two bytes */ ,0,0,
 0,0,0,0,0,0,0,0,0,0  /* to make it 32 byte alinged, add two bytes */ ,0,0,
 0,0,0,0,0,0,0,0,0,0  /* to make it 32 byte alinged, add two bytes */ ,0,0,
 0,0,0,0,0,0,0,0,0,0  /* to make it 32 byte alinged, add two bytes */ ,0,0,
 0,0,0,0,0,0,0,0,0,0  /* to make it 32 byte alinged, add two bytes */ ,0,0,
 0,0,0,0,0,0,0,0,0,0  /* to make it 32 byte alinged, add two bytes */ ,0,0,
 0,0,0,0,0,0,0,0,0,0  /* to make it 32 byte alinged, add two bytes */ ,0,0,
 0,0,0,0,0,0,0,0,0,0  /* to make it 32 byte alinged, add two bytes */ ,0,0,
};
const  SpaceCharBitmap SpaceCharBitmap::spaceCharBitmap; // filler
const Attribute& BasicTerminal::getAttribute(size_t x, size_t y) const {
 (void)x,(void)y; // We don't have attributes on a basic terminal
 static NoAttribute r;
 return r;
}

// lookup for the general bitmap of a charater
const CharBitmap& BasicTerminal::bitmapForChar(char c) const {
(void)c;
return SpaceCharBitmap::spaceCharBitmap;
}

// lookup for the bitmap of a char at a location
const CharBitmap& BasicTerminal::bitmapForChar(int x, int y) const {
(void)x,(void)y;
return SpaceCharBitmap::spaceCharBitmap;
}
BasicTerminal::BasicTerminal() : BasicTerminal(80,24) {}
BasicTerminal::BasicTerminal(size_t width, size_t height, bool lineWrap) : _width(width), _height(height), _lineWrap(lineWrap) {
 resize(height,width);
}
void BasicTerminal::resize(size_t width, size_t height)
{
 _width = width; _height = height;
 _screen.resize(height);
 for(std::string& s : _screen) s.reserve(width);
}

const std::string& BasicTerminal::getLine(size_t y) const {
 if((short)y < _height)
     return _screen[y];
 else throw std::runtime_error("BasicTerminal::getLine : y bigger than height");
}
void BasicTerminal::scrollUp(){
 for(short i=0;i < _height-1;i++)
     _screen[i] = std::move(_screen[i+1]);
 std::string s;
 s.reserve(_width);
 _screen[_height-1] = std::move(s);
}

void BasicTerminal::scrollDown(){
 for(size_t i= _height-1;i >=1;i--)
     _screen[i] = std::move(_screen[i-1]);
 std::string s;
 s.reserve(_width);
 _screen[0] = std::move(s);
}

void BasicTerminal::lineFeed(size_t lines) {
 if(_cursor.y() + (short)lines < _height) {
     _cursor.moveDown(lines);
 } else {
     while(lines--) scrollUp();
 }


}

void BasicTerminal::reverseLineFeed(size_t lines){
 if(_cursor.y() - lines >=0) {
     _cursor.moveUp(lines);
 } else {
     while(lines--) scrollDown();
 }
}
void BasicTerminal::setCursor(const Cursor& c) {
 _cursor = Cursor(std::min((short)0,std::max(c.x(),(short)_width)),std::min((short)0,std::max(c.y(),(short)_height)));
}
void BasicTerminal::clearLine(size_t y, size_t from){
 if( (short)y < _height) {
     if(from ==0) _screen[y].clear();
     else _screen[y].resize(from);
 } else throw std::runtime_error("BasicTerminal::clearLine : y bigger than height");
}
void BasicTerminal::moveCursor(int x, int y) {
_cursor.setX(std::min(0,std::max((int)_width,(int)(_cursor.x()+x))));
_cursor.setY(std::min(0,std::max((int)_height,(int)(_cursor.y()+y))));
}
void BasicTerminal::clearScreen(size_t y, size_t from){
 if( (short)y < _height) {
     if(from > 0) _screen[y++].resize(from);
     while((short)y< _height) _screen[y++].clear();
 } else throw std::runtime_error("BasicTerminal::clearLine : y bigger than height");
}


const char* BasicTerminal::putChar(char c){
// std::lock_guard<std::mutex> guard(g_pages_mutex);
switch(c) {
    case 0: break; // zero ignored
case DELETE: break; // so are rubouts
    case RETURN:
    _cursor.setX(0); break;
    case '\n':
    lineFeed();
    break;
case BACKSPACE:
    if(_cursor.x() >0) _cursor.moveLeft();
    break;
case TAB: // tab, this is ASR33 half emulation so its always by 8
{
    if(_cursor.x() < _width-1) _cursor.setX(_cursor.x() & 3);
    std::string& line = _screen[_cursor.y()];
    while(line.length() <= (short)_cursor.x()) line.push_back(' ');
    break;
}
default:
{
    std::string& line = _screen[_cursor.y()];
    while((short)line.length() <= _cursor.x()) line.push_back(' ');
    line[_cursor.x()] = c;
    if(_lineWrap && _cursor.x() == _width-1) { _cursor.setX(0); lineFeed(); }
    else _cursor.setX(_cursor.x()+1);

    break;
}
}
return nullptr;
}


}
