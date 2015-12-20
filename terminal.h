#ifndef TERMINAL_H
#define TERMINAL_H

#include <iostream>
#include <vector>
#include <cstdint>
#include <functional>
#include <thread>
#include <chrono>
#include <memory>
#include <atomic>
#include <array>

namespace Terminal {

class Cursor {
    short _x;
    short _y;
public:
    Cursor() : _x(0), _y(0) {}
    Cursor(short x, short y) : _x(x), _y(y) {}
    inline void moveUp(short i=1) { _y-=i; }
    inline void moveDown(short i=1) { _y+=i; }
    inline void moveRight(short i=1) { _x=+i; }
    inline void moveLeft(short i=1) { _x-=i;}
    inline short x() const { return _x; }
    inline short y() const { return _y; }
    inline void setX(short v) { _x= v; }
     inline void setY(short v) { _y= v; }
    inline bool operator==(const Cursor& l) const { return _x == l._x && _y == l._y; }
    inline bool operator!=(const Cursor& l) const { return !(l == *this); }
    inline Cursor& operator+=(const Cursor& l) { _x += l._x; _y+= l._y; return *this; }
    inline Cursor& operator-=(const Cursor& l) { _x -= l._x; _y-= l._y; return *this; }
};

class Box {
    Cursor _upperLeft;
    Cursor _bottomRight;
public:
    Box() : _upperLeft(0,0), _bottomRight(0,0) {}
    Box(size_t x, size_t y, size_t width, size_t height) : _upperLeft(x,y), _bottomRight(x + width-1,y+height-1) {}
    Box(const Cursor& upperLeft, const Cursor& bottomRight) : _upperLeft(upperLeft), _bottomRight(bottomRight) {}
    inline size_t x() const { return _upperLeft.x(); }
    inline size_t y() const { return _upperLeft.y(); }
    inline size_t height() const { return _bottomRight.y() - _upperLeft.y() +1; }
    inline size_t width() const { return _bottomRight.x() - _upperLeft.x() +1; }
    inline void translate(const Cursor& c) { _upperLeft += c; _bottomRight +=c; }
    inline Box& operator+=(const Box& l) { _upperLeft += l._upperLeft; _bottomRight+= l._bottomRight; return *this; }
    inline Box& operator-=(const Box& l) { _upperLeft -= l._upperLeft; _bottomRight-= l._bottomRight; return *this; }
};

struct Attribute {
    virtual bool underline() const =0;
    virtual bool blink() const =0;
    virtual bool bold() const =0;
    virtual bool inverted() const =0;
    virtual bool altChar() const =0;
    virtual bool doubleheight() const =0;
    virtual bool doublewidth() const =0;
    virtual size_t foreground() const =0; // colors, diffrent on diffrent emulation sets
    virtual size_t background() const =0;
    // if we have a bitmap of this char
    virtual const unsigned char* bitmap() const { return nullptr; }
    virtual ~Attribute() {}
};
// this is a charbitmap of the current charater being shown depending on the mode we are in
// the color of a bitmap depends on the emulation, for BasicTerminal 0 is background and 1 is forground

struct CharBitmap {
    virtual const unsigned char* scanline(size_t y) const =0;
    virtual const unsigned char* data() const =0;
    virtual size_t height() const =0;
    virtual size_t width() const =0;
    inline virtual size_t bytesAligned() const { return sizeof(uint32_t); } // normaly 32 bit alined
    inline virtual size_t scanlineLength() const { return (width() / bytesAligned()) + bytesAligned();  } // normaly 32 bit alined
    virtual int charCode() const = 0;
    virtual const Attribute& attribute() const =0;
};
// simple 16 byte FIFO buffer for the serial port
// Copied from https://bitbucket.org/KjellKod/lock-free-wait-free-circularfifo/src
// It will be the primary method of passing data between two threaded devices using
// the serial port interface.

class FIFOBuffer{
    static const size_t FIFO_SIZE = 16; // kind of standard for serial ports
    static const size_t FIFO_MASK = FIFO_SIZE-1; // kind of standard for serial ports
    std::array<uint8_t,FIFO_SIZE>  _array;
    std::atomic_uint8_t _head;
    std::atomic_uint8_t _tail;
    inline uint8_t increment(uint8_t v) const { return (v+1) & FIFO_MASK; }
public:
    FIFOBuffer() : _head(0), _tail(0) {}
    inline bool push(uint8_t c) {
        const uint8_t current_tail = _tail.load(std::memory_order_relaxed);
         const uint8_t next_tail = increment(current_tail);
         if(next_tail != _head.load(std::memory_order_acquire)){
           _array[current_tail] = c;
           _tail.store(next_tail, std::memory_order_release);
           return true;
         }
         return false; // full queue
    }
    inline bool pop(uint8_t& c) {
        const uint8_t current_head = _head.load(std::memory_order_relaxed);
          if(current_head == _tail.load(std::memory_order_acquire)) return false; // empty queue
          c = _array[current_head];
          _head.store(increment(current_head), std::memory_order_release);
          return true;
    }
    inline bool wasEmpty() const {
        return _head.load() == _tail.load();
    }
    inline bool wasFull() const {
        const uint8_t next_tail = increment(_tail.load());
        return (next_tail == _head.load());
    }
};

class SerialDCEInterface {
    // Terminating equipment.  This is the computer. You trasmit to the computer and recive from
    const uint8_t XOFF = 0x13;
    const uint8_t XON = 0x11;
protected:
    std::atomic_uint32_t _band;
    FIFOBuffer _trasmitBuffer;
    FIFOBuffer _recivedBuffer;
    std::function<void(SerialDCEInterface&)>  _callback;
protected:
    bool internalTrasmit(uint8_t c) {
        bool b = _trasmitBuffer.push(c); // handle overflow somehow
        if(_callback) _callback(*this);
        return b;
    }
    bool internalRecive(uint8_t& c) { return _recivedBuffer.pop(c); }
    // internal function
public:
    SerialDCEInterface() : _band(110),_callback(nullptr) {}
    void setCallBack(std::function<void(SerialDCEInterface&)>  callback) { _callback = callback; }
    void setBaundRate(uint32_t band) { _band = band; }
    virtual bool trasmit(uint8_t c) { return _recivedBuffer.push(c); }
    virtual bool recive(uint8_t &c) { return _trasmitBuffer.pop(c);  }
};
typedef std::function<void(SerialDCEInterface&)> SerialDCEInterfaceCallback;


class BasicTerminal { // base class of them all
public:
    // These both are light weight classes mainly used for cursor positioning and text
    // selecting


protected:
    std::vector<std::string> _screen;
    Cursor _cursor;
    short _width;
    short _height;
    bool _lineWrap;
protected:
    virtual void scrollDown();
    virtual void scrollUp();
public:
    BasicTerminal();
    BasicTerminal(size_t width, size_t height,bool lineWrap=false);
    void resize(size_t width, size_t height);
    const std::string& getLine(size_t y) const;
    const Attribute& getAttribute(size_t x, size_t y) const;
    const int getHeight() const { return _height; }
    const int getWidth() const { return _width; }
     void setCursor(const Cursor& c);
     void moveCursor(int x, int y);
    Cursor getCursor() const { return _cursor; }
    void setLineWrap(bool lineWrap) { _lineWrap = lineWrap; }
    bool getLineWrap() const { return _lineWrap; }
    virtual void lineFeed(size_t lines=1);
    virtual void reverseLineFeed(size_t lines=1);
    virtual void clearLine(size_t y, size_t from=0);
    virtual void clearScreen(size_t y=0, size_t from=0);

    // lookup for the general bitmap of a charater
    virtual const CharBitmap& bitmapForChar(char c) const;

    // lookup for the bitmap of a char at a location
    virtual const CharBitmap& bitmapForChar(int x, int y) const;


    // returns null if nothing is auto sent out, otherwise
    // it will return a null terminated string
    // aka when the terminal responds to an
    // ident message
    // its up to the usere to make sure the message is sent
    // to the right place
    virtual const char* putChar(char c);
};

}

#endif // TERMINAL_H
