#ifndef TIMER_H
#define TIMER_H



#include <cstdint>
#include <thread>
#include <chrono>
#include <memory>
#include <atomic>
#include <queue>
#include <hash_set>
#include <functional>
#include <QDebug>

class SimpleThread {
    std::atomic<bool> _die;
    std::atomic<bool> _dead;
    void _threadRun() {
        _die = _dead = false;
        qDebug() << "Starting Simple Thread";
        while(!_die) threadFunction();
        _dead = true;
        qDebug() << "Ending Simple Thread";
    }

protected:
    virtual bool threadFunction() = 0;
    void startThread() { if(!_dead) std::thread(&SimpleThread::_threadRun,this).detach(); }
    void stopThread() { _die = true; while(!_dead); }

public:
    SimpleThread() : _die(false), _dead(false) {
        //std::thread(&SimpleThread::_threadRun,this).detach();
    }
    inline bool threadRunning() const { return !_dead; }
    virtual ~SimpleThread() {
        stopThread();
    }
};


// async runs the function in detach


class TimerQueue {
public:
    static const int64_t microsecond= 5l;
    static const int64_t millisecond= 5000l;
    static const int64_t second= 5000000l;
    static const int64_t gcountdown_max = 9223372036854775807L;
    typedef std::function<void()>  TimerAction;
    class Timer {
        int64_t _delay;
        TimerAction _action;
        bool _async;
        Timer* _next;
    public:
        Timer(int64_t delay, TimerAction action, bool async = false) : _next(nullptr), _delay(delay), _action(action),_async(async) {}
        inline int64_t delay() const { return _delay; }
        inline TimerAction action() const { return _action; }
        inline bool async() const { return _async; }
        inline void runAction() const { std::thread t(_action); if(_async) t.detach(); else t.join(); }
        bool operator==(const Timer& t) { return _delay == t._delay; }
        bool operator<(const Timer& t) { return _delay > t._delay; } // sorted from behind
        friend class TimerQueue;
    };
private:
   //
    Timer* _head;
    int64_t _countdown;
    void _timerThreadFunction();
protected:
    bool threadFunction();

    TimerQueue() { }
    ~TimerQueue() { }
    int64_t countdown() const { return _countdown; }
    void fire_timer();
    int64_t query(const Timer* timer) const ;
    void schedule(Timer* timer) { schedule(timer,timer->delay()); }
    void schedule(Timer* timer, int64_t delay);
};


#endif // TIMER_H
