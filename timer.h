#ifndef TIMER_H
#define TIMER_H



#include <cstdint>
#include <thread>
#include <chrono>
#include <memory>
#include <atomic>
#include <queue>
#include <functional>
#include <QDebug>
#include <mutex>
#include <condition_variable>

class SimpleThread {
    std::atomic<bool> _die;
    std::atomic<bool> _dead;
    std::mutex _mutex;
    std::condition_variable _thread_died;

    void _threadRun() {
        std::unique_lock<std::mutex> lk(_mutex);
        _die.store(false);
        _dead.store(false);
        qDebug() << "Starting Simple Thread";
        while(!_die.load() && threadFunction());
        if(!_die.load()) threadStopped();
        _dead = true;
        qDebug() << "Ending Simple Thread";
        _thread_died.notify_all();
    }
protected:
    std::mutex& mutex() { return _mutex; }
    virtual bool threadFunction() = 0;
    virtual void threadStopped() {} // callback when thread function stops thread
    void startThread() {
        if(!_dead) std::thread(&SimpleThread::_threadRun,this).detach();
        else throw std::runtime_error("Thread not dead");
    }
    void stopThread() {
        std::unique_lock<std::mutex> lk(_mutex);
        _die.store(true);
        if(_thread_died.wait_for(lk,std::chrono::milliseconds(500))== std::cv_status::timeout) {
            throw std::runtime_error("Thread cannot die?");
            _thread_died.wait(lk);
        }
    }
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
