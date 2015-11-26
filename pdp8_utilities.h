#ifndef PDP8_UTILITIES_H
#define PDP8_UTILITIES_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <exception>
#include <functional>
#include <chrono>
#include <future>
//#include <cstdio>
#include "pdp8state.h"

// This is a simple protected setter getter macro maker
// Its getting a bit unwhilding with all the getters/setters and thought this make it a tad
// easyer
#define GETSETVALUE(type,name) protected: type _##name; public: inline type name() const { return _##name; } public: inline type& name()  { return _##name; } public: inline void name(type v) { _##name = v; }


// Originaly I was going to do a queued single threaded process but
// to be honest, it was going to be MORE effort than to use the
// standard C++11 stuff.  So the Device class has a built in
// timer thread.  You can set it to run the task() function on each
// time interval or set it to just run constantly
class SimpleTimer
{
    typedef std::chrono::high_resolution_clock clock_t; // change if we don't support this
    std::chrono::microseconds _interval;
    std::atomic<bool> _run; // do I really need to do ths?
    std::atomic<bool> _running; // do I really need to do ths?
    std::function<void()> _func;
    GETSETVALUE(int,test)
public:
    SimpleTimer() {}
    ~SimpleTimer() {
        while(_running);
    }
    inline bool running() const { return _running; }
    inline void setDelay(std::chrono::microseconds v) { _interval = v; }
     inline void setDelay(std::chrono::milliseconds v) { _interval = v; }
    void setInterval(size_t after) { _interval = std::chrono::milliseconds(after); }
    void setFunction(std::function<void()> func) { _func = func; }
    void start() {
        if(_run || _running) return;
        if(!_func) return;
        if(_interval == std::chrono::microseconds::zero()) return;
        assert(_run.is_lock_free());
        std::thread([this]{
            _running = _run = true;
            while(_run) { // mabey pause would work better  std::this_thread::sleep_for(ydiff);
                auto start = clock_t::now();
                if(_func) _func();
                auto end = clock_t::now();
                 auto mdiff = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
                 auto ydiff = std::chrono::duration_cast<std::chrono::milliseconds>(_interval-mdiff);
                 if(ydiff.count() >0)  std::this_thread::sleep_for(ydiff);
                 else throw new std::runtime_error("Task too long");
            }
            _running = false;
        }).detach();
    }
    void stop() {
        if(!_run || !_running) return;
        _run = false;
    }


};
// Sort of like a resetable one shot event
// Used in TTY, might be useful in other programs, if not I will just combine it with something else
class SimpleEvent {
    typedef std::chrono::high_resolution_clock clock_t; // change if we don't support this
    std::atomic<bool> _running;
    std::chrono::microseconds _pause;
    std::function<void()> _func;
public:
    SimpleEvent() : _running(false) , _pause(std::chrono::microseconds::zero()),_func(nullptr) {}
    inline bool running() const { return _running; }
    inline void setDelayMillseconds(uint32_t v) { _pause = std::chrono::milliseconds(v); }
    inline void setDelayMicroseconds(uint32_t v) { _pause = std::chrono::microseconds(v); }
    inline void setDelay(std::chrono::microseconds v) { _pause = v; }
     inline void setDelay(std::chrono::milliseconds v) { _pause = v; }
    inline bool start() {
        if(_running) return false;
        if(!_func) throw std::runtime_error("SimpleEvent: No run function defined");
         assert(_running.is_lock_free());
        std::thread([this](){
            _running = true;
             std::this_thread::sleep_for(_pause);
             _func();
             _running = false;
        }).detach();
        return true;
    }
    void setFunction(std::function<void()> func) { _func = func; }
};

namespace PDP8 {
class SimpleBuffer {
    std::queue<unsigned char> _data;
public:
    void reset() { while(!_data.empty()) _data.pop(); }
    void putData(unsigned char c) { _data.push(c); }
    int  getData() {
        if(!_data.empty()) {
            unsigned char c = _data.front();
            _data.pop();
            return c;
        }
        return 0;
    }
    bool haveData() const { return !_data.empty(); }
};
std::string to_octal(int num,const char* fmt="%04o") ;
inline bool isIndirect(uint16_t a) { return a & 0x100; }
inline bool isCurrent(uint16_t a) { return a & 0x80; } //0200
class octzero {

    int _num;
     int _digits;
   // int _digits;
  //  bool _zero;
public:
    octzero(int num,int digits = 4) : _num(num),_digits(digits) {}
    friend std::ostream& operator<< (std::ostream& stream, const octzero& o);
};
std::ostream& operator<< (std::ostream& s, const octzero& o) ;


    class TwelveBit { // right now just for adder tester
        short _d  : 12;
    public:
        template<typename T> inline TwelveBit(T v) : _d(static_cast<short>(v)) {}
    #define AOPR(b) template<typename T> inline TwelveBit operator b(T a) { _d b static_cast<short>(a); return *this; }  inline TwelveBit operator b(TwelveBit a) { _d b a._d; return *this; }
        AOPR(=) AOPR(+=)AOPR(-=) AOPR(|=) AOPR(&=) AOPR(^=)
    #undef AOPR


    };
#ifndef _NOEXCEPT
#define noexcept
#endif

    // make some better ones
    class PDP8_Exception : std::exception {
        std::string msg;
    public:
        PDP8_Exception(const std::string& msg) : msg(msg) {}
        const char* what() const throw() { return msg.c_str(); }

    };
    void LoadRim(std::istream& i ,CpuState& s, size_t loc =0200);
    void LoadRim(const std::string& filename, CpuState& s, size_t origin =0200);
    void LoadBin(std::istream& i ,CpuState& s, size_t loc =0200);
    void LoadBin(const std::string& filename, CpuState& s, size_t origin =0200);

    void LoadPaperTape(const std::string& filename, CpuState& s, size_t origin =0200);





    struct FileBuffer {
        std::vector<char> m_vec;
        size_t m_index;
        bool _open;
        void openFile(const char* name) {
            m_index = 0;
            m_vec.clear();
            FILE* f = fopen(name,"rb");
            fseek(f, 0, SEEK_END); // seek to end of file
            size_t size = ftell(f); // get current file pointer
            fseek(f, 0, SEEK_SET); // seek back to beginning of file

            m_vec.resize(size);
            fread(&m_vec[0],sizeof(char),size,f);
            fclose(f);
            _open = true;
        }
        operator bool() const { return _open; }
        void write(char* p, size_t size)
        {
            if(eof())
                throw std::runtime_error("Premature end of array!");

            if((m_index + size) > m_vec.size())
                throw std::runtime_error("Premature end of array!");

            std::memcpy(reinterpret_cast<void*>(&m_vec[m_index]), p, size);

            m_index += size;
        }
         template<typename T>
        void write(const T& t)
        {
            std::vector<char> vec(sizeof(T));
            std::memcpy(reinterpret_cast<void*>(&vec[0]), reinterpret_cast<const void*>(&t), sizeof(T));
            write(vec);
        }

        FileBuffer() : m_index(0) {}
           FileBuffer(const char * mem, size_t size)
           {
               open(mem, size);
           }
           FileBuffer(const std::vector<char>& vec)
           {
               m_index = 0;
               m_vec.clear();
               m_vec.reserve(vec.size());
               m_vec.assign(vec.begin(), vec.end());
           }
           void open(const char * mem, size_t size)
           {
               m_index = 0;
               m_vec.clear();
               m_vec.reserve(size);
               m_vec.assign(mem, mem + size);
           }
           void close()
           {
               m_vec.clear();
           }
           bool eof() const
           {
               return m_index >= m_vec.size();
           }
           std::ifstream::pos_type tellg()
           {
               return m_index;
           }
           bool seekg (size_t pos)
           {
               if(pos<m_vec.size())
                   m_index = pos;
               else
                   return false;

               return true;
           }
           bool seekg (std::streamoff offset, std::ios_base::seekdir way)
           {
               if(way==std::ios_base::beg && offset < m_vec.size())
                   m_index = offset;
               else if(way==std::ios_base::cur && (m_index + offset) < m_vec.size())
                   m_index += offset;
               else if(way==std::ios_base::end && (m_vec.size() + offset) < m_vec.size())
                   m_index = m_vec.size() + offset;
               else
                   return false;

               return true;
           }

           const std::vector<char>& get_internal_vec()
           {
               return m_vec;
           }

           template<typename T>
           void read(T& t)
           {
               if(eof())
                   throw std::runtime_error("Premature end of array!");

               if((m_index + sizeof(T)) > m_vec.size())
                   throw std::runtime_error("Premature end of array!");

               std::memcpy(reinterpret_cast<void*>(&t), &m_vec[m_index], sizeof(T));

               m_index += sizeof(T);
           }
           void readPDP8(unsigned short* p, size_t size) {
               if(eof())
                   throw std::runtime_error("Premature end of array!");

               if((m_index + size) > m_vec.size())
                   throw std::runtime_error("Premature end of array!");

               for(size_t i=0;i<size;i++)
                   p[i] = m_vec[m_index++];

           }

           void read(char* p, size_t size)
           {
               if(eof())
                   throw std::runtime_error("Premature end of array!");

               if((m_index + size) > m_vec.size())
                   throw std::runtime_error("Premature end of array!");

               std::memcpy(reinterpret_cast<void*>(p), &m_vec[m_index], size);

               m_index += size;
           }
    };



}
#endif // PDP8_UTILITIES_H
