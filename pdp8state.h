#ifndef PDP8STATE_H
#define PDP8STATE_H

#include <iterator>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <memory>
#include <atomic>

namespace PDP8 {
enum class State {
    Fetch,
    Defer,
    Execute,
    Iot,
    Opr
};
struct Regesters {
    int32_t pc;
    int32_t ea;
    int16_t ir;
    int16_t opnd;
    int16_t lac;
    int16_t mq;
    int16_t lfr;
    int16_t ma;
    int16_t mb;
    int16_t svr;
    char ibr;
    char ifr;
    char dfr;
};
class RegesterHistory {
    static const size_t HIST_COUNT = 64;
    static const size_t HIST_MASK = (HIST_COUNT-1);
    Regesters hst[HIST_COUNT];
    size_t _pos;
    size_t _count;
public:
    RegesterHistory() : _pos(0), _count(0) {}
    inline int32_t count() const { return _count; }
    inline void clear()  { _pos = _count = 0; }
    void push(const Regesters& r);
    inline const Regesters& operator[](int i) const { return hst[(_pos-i) & HIST_MASK]; }
    const Regesters* begin() const { return &hst[_pos-1& HIST_MASK]; }
    const Regesters* end() const { return &hst[_pos-1+_count& HIST_MASK]; }
};

typedef std::vector<uint16_t> MainMemory;

class CpuState;

// Originaly I was going to do a queued single threaded process but
// to be honest, it was going to be MORE effort than to use the
// standard C++11 stuff.  So the Device class has a built in
// timer thread.  You can set it to run the task() function on each
// time interval or set it to just run constantly
class SimpleTimer
{
    typedef std::chrono::high_resolution_clock clock_t; // change if we don't support this
    std::chrono::milliseconds _interval;
    std::atomic<bool> _run; // do I really need to do ths?
    std::atomic<bool> _running; // do I really need to do ths?
    std::unique_ptr<std::thread> _thread;
    std::function<void()> _func;
    void _threadLoop() {
        _running = _run = true;
        assert(_run.is_lock_free());
        while(_run) {
            auto start = clock_t::now();
            if(_func) _func();
            auto end = clock_t::now();
             auto mdiff = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
             auto ydiff = std::chrono::duration_cast<std::chrono::milliseconds>(_interval-mdiff);
             if(ydiff.count() >0)  std::this_thread::sleep_for(ydiff);
             else throw new std::runtime_error("Task too long");
        }
        _running = false;
    }

public:
    SimpleTimer() {}
    ~SimpleTimer() {
        if(_run) {
            _run = false;
            while(_running);
        }
    }

    void setInterval(size_t after) { _interval = std::chrono::milliseconds(after); }
    void setFunction(std::function<void()> func) { _func = func; }
    void start() {
        if(_run || _running) return;
        if(!_func) return;
        if(_interval == std::chrono::milliseconds::zero()) return;
        if(_thread) return;
        _thread.reset(new std::thread(&SimpleTimer::_threadLoop,this));
        _thread->detach();
    }
    void stop() {
        if(!_run || !_running) return;
        if(!_thread) return;
        _run = false;
        _thread->join();
        _thread.release();
    }


};


class Device {
protected:
    CpuState& c;
    Device(CpuState& c);
public: // abstract interface
    virtual void clear() = 0;
    virtual void iot() = 0;
    virtual void updateInterrupts()=0;
    virtual std::string debug() const = 0;
     virtual ~Device() {}

};
typedef std::shared_ptr<Device> DevicePtr;
enum class PanelToggleSwitch {
    Start,
    Stop,
    Cont,
    SingleStep,
    Dep,
    Exam,
    LoadAdd
};

class CpuState {
protected:
    static const size_t MAX_MEMORY = 32768;
    Regesters r;
    MainMemory m;
    uint32_t _sw;
    State _state;

    bool _run;      // the state of the engine
    bool _running; // on if the thread is running
    bool _singleStep;
    bool _interrupt_enable;
    bool _interrupt_enable_delay;
    uint64_t _intrupet_request; // one of these bits are set, then we have a request
    bool _in_interrupt;
    bool _skip;
    DevicePtr _iots[64];
    friend class Device;
public:
    CpuState();

public: // diffrent switches
    // when the power switch is turned, this is turned on
    // clear eveything, like a power reset
    // carful, power clears EVEYTHING EXCEPT memory
    void power();

    inline DevicePtr findIot(size_t pos) { return _iots[pos]; }


    // when the start key is pressed
    inline void setSR(uint32_t i) { _sw = i; } // set the SR switch
    inline uint32_t getSR() const { return _sw; }
    void panelSwitch(PanelToggleSwitch s); // most are toggle so might not use state much


public: // getters and setters
    inline Regesters& regs() { return r; }
    inline bool haveInterrupt() const { return _interrupt_enable && _interrupt_enable_delay &&  _intrupet_request !=0; } // temp
    inline bool interruptsEnabled() const { return _interrupt_enable && _interrupt_enable_delay;}
    inline unsigned short& operator[](int v) { return m[v]; }
    inline bool run() const { return _run; }
    inline bool running() const { return _running; }
    inline bool singleStep() const { return _singleStep; }
    inline bool skip() const { return _skip; }
    inline void setSkip() { _skip = true; }
    inline State state() const { return _state; }
    inline void setInterrupt(char dev) { _intrupet_request |= ((uint64_t)1 << dev); }
    inline void clearInterrupt(char dev) { _intrupet_request &= ~((uint64_t)1 << dev); }
    friend class Cpu; // frend of the CPU class since that calls can screw with anything
    friend class Emx8;

};

}



#endif // PDP8STATE_H
