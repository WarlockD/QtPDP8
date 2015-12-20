#ifndef INCLUDES
#define INCLUDES

//includes for most things

#include <iostream>
#include <cassert>
#include <thread>
#include <queue>
#include <chrono>
#include <memory>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <mutex>          // std::mutex
#include <condition_variable>
#include <atomic>
#include <locale>
#include <codecvt>
#include <stdio.h>
#include <stdlib.h>
#include <atomic>
#include <set>

#include <QString>
#include <QDebug>
namespace PDP8 {
    void debug_print(const char* s, ...);
class CpuState;
class Cpu;
typedef signed char     int8;
typedef unsigned char   byte;
typedef signed short    int16;
typedef signed int      int32;
typedef unsigned char   uint8;
typedef unsigned short  uint16;
typedef unsigned int    uint32;

typedef std::ratio<1, 10000000> ratio_timeslice;
typedef std::chrono::duration<long long, ratio_timeslice> time_slice;
typedef std::chrono::nanoseconds time_ns;
typedef std::chrono::microseconds time_us;
typedef std::chrono::milliseconds time_ms;
// Made a custom duration taht is 100ns slices.
// From the timing in this thing, we don't need PURE nanosecond timing
// but on the other hand we need to be able to count atleast half micro ticks
// also a bunch of typedefs so we can use these around and not
// keep typing std::chrono::looongtimedef all the time.
typedef std::chrono::high_resolution_clock pdp8_clock;
typedef pdp8_clock::time_point time_point; // cast for this
#define TIMEZERO (time_point::duration::zero())

// simple debugging stream
class Log : public std::wostream {
    class PDP8LogBuffer : public std::wstreambuf {
        typedef std::wstring string_type;
        typedef std::wstreambuf::traits_type traits;   // shortcut for a string
        string_type _buffer;
        string_type _header;
    public:
        PDP8LogBuffer()  {}
        void setHeadder(string_type && header) {
            _header = std::move(header);
        }
        void setHeadder(const char* mbstr) {
            std::mbstate_t state = std::mbstate_t();
            size_t len = 1 + std::mbsrtowcs(nullptr, &mbstr, 0, &state);
            std::vector<wchar_t> wstr(len);
            std::mbsrtowcs(&wstr[0], &mbstr, wstr.size(), &state);
            _header = std::wstring(wstr.begin(),wstr.end());
        }
        void setHeadder(const wchar_t* mbstr) {
            _header = mbstr;
        }
        ~PDP8LogBuffer() { pubsync(); }
        traits::int_type overflow(traits::int_type  c) override {
            if(c !=  traits::eof()){
                // We want to flush the buffer at the end of each line
                // as most debuggers add a \n anyway
                if(c == traits::to_int_type('\n')) sync();
                else _buffer += traits::to_char_type(c);

            }
            return traits::not_eof(c);
        }
        int sync() override {
            if(!_buffer.empty()) {
                if(_header.empty())
                    qDebug() << QString::fromStdWString(_buffer);
                else
                    qDebug() << QString::fromStdWString(_header) << ": " << QString::fromStdWString(_buffer);;
            }
            _buffer.clear();
            return 0;
        }
    } _buff;
public:
    Log() : std::wostream(&_buff) {}
    Log(const char* file, int line) : std::wostream(&_buff){
        char _buffer[64];
        sprintf(_buffer,"%10s(%3d)",file,line);
        _buff.setHeadder(_buffer);
    }
    Log(const wchar_t* function, const wchar_t* file, int line) : std::wostream(&_buff){
        wchar_t _buffer[256]; // fucntion seems to be in unicode, ugh
        swprintf(_buffer,256,L"%10s(%3d) at %s",file,line,function);
        _buff.setHeadder(_buffer);
    }
};
#define __STR2WSTR(str) L##str

#define _STR2WSTR(str) __STR2WSTR(str)
#ifndef __FUNCTIONW__
#define __FUNCTIONW__ _STR2WSTR(__FUNCTION__)
#endif
#ifndef __FILEW__

#define __FILEW__ _STR2WSTR(__FILE__)
#endif
#define LOGF() Log(__FUNCTIONW__ , __FILEW__, __LINE__)
/*
#define CTXRAWLOG(text) DebugLogger(text).get()
#define CTXLOG(text) DebugLogger(text, __FILE__, __LINE__).get()
#define FLOG() DebugLogger(__FUNCSIG__, __FILE__, __LINE__).get()
#define LOG() DebugLogger(__FILE__, __LINE__).get()
*/

/*
typedef std::chrono::duration_cast<time_slice> cast_slice;
typedef std::chrono::duration_cast<time_ns> cast_ns;
typedef std::chrono::duration_cast<time_us> cast_us;
typedef std::chrono::duration_cast<time_ms> cast_ms;
*/
    struct int12 {
        int16 _v;
    public:
        int12() : _v(0) {}
        int12(int16 v) : _v(v&0xFFF) {}
        const int16 value() const { _v; }
        template<int B> inline bool bit() const { return _v & (1<<B) ? true : false; }
        inline int12 operator=(const int12 v) { _v = v._v;  return *this; }

        inline int12 operator+=(const int12 v) { _v += v._v; _v&=0xFFF; return *this; }
        inline int12 operator-=(const int12 v) { _v -= v._v; _v&=0xFFF; return *this; }
        inline int12 operator/=(const int12 v) { _v /= v._v; _v&=0xFFF; return *this; }
        inline int12 operator*=(const int12 v) { _v *= v._v; _v&=0xFFF; return *this; }
        inline bool operator==(const int12 v) const { return _v == v._v; }
        inline bool operator!=(const int12 v) const { return !(v == *this); }
        inline bool operator<(const int12 v) const { return _v < v._v; }
        inline bool operator>(const int12 v) const { return _v > v._v;}
        // humm might not need but its a nifty idea for doing multipule types
        template<typename T,typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
        inline int12 operator=(const T v) { _v = (v&0xFFF); return *this; }
        template<typename T,typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
        inline int12 operator+=(const T v) { _v = (_v + v)&0xFFF; return *this; }
        template<typename T,typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
        inline int12 operator-=(const T v) { _v = (_v - v)&0xFFF; return *this; }
        template<typename T,typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
        inline int12 operator/=(const T v) { _v = (_v / v)&0xFFF; return *this; }
        template<typename T,typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
        inline int12 operator*=(const T v) { _v = (_v * v)&0xFFF;return *this; }
        template<typename T,typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
        inline int12 operator&=(const T v) { _v = (_v & v)&0xFFF;return *this; }
        template<typename T,typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
        inline int12 operator|=(const T v) { _v = (_v | v)&0xFFF;return *this; }
        // equals works?
        template<typename T,typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
        inline bool operator==(const T v) const { return _v == (v&0xFFF); }
        template<typename T,typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
        inline bool operator!=(const T v) const { return _v != (v&0xFFF); }
    };
    struct InstructionDecode {

    };
    enum class RunningState : int {
         Halt=-1,   // when the cpu makes a halt op
        Stop=0,
        SingleStepInstruction,
        SingleStepState,
        Run,
        // These are debugging functions
        //StopThread, // This is used to kill the thread

    };
}
struct ostr {
    int z;
    int n;
    ostr(int z,int n=6) : z(z),n(n) {}
};
std::ostream& operator<<(std::ostream& s, const ostr& o);
void writedebug(const char* fmt,...);
void writedebug(const std::string& sr);
#endif // INCLUDES

