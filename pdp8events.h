#ifndef PDP8EVENTS_H
#define PDP8EVENTS_H
#include "includes.h"


// so we are making an event queue, its a thing
// The purpose of the events queue is to act like the bus
// diffrent devices send messages to here that sync with the cpu
// so in essence this acts like the bus that operates independently of the
// CPU and devices


namespace PDP8 {

/* Convert switch letter to bit mask */

    // The more I look at this, the more it feels like a a
    // Singlily linked list would be faster.
    // still, the std:: priority queue handles things so much cleaner!
    typedef std::ratio<1, 10000000> ratio_timeslice;
    typedef std::chrono::duration<long long, ratio_timeslice> timeslice;


// return a time to start another event again at
typedef std::function<time_slice()> EventFunction;
   typedef uint32_t EventID;
    class EventSystem {
        // I am doing alot to use std::queue.  Is it really worth it?
    class SimEvent {
        time_slice _time;
        const EventFunction* _event;
    public:
        SimEvent(const EventFunction* e,time_slice time) : _time(time), _event(e) {}
        SimEvent(const SimEvent& e,time_slice time) : _time(time), _event(e._event) {}
        time_slice execute() const {  return (*_event)(); }
        time_slice time() const { return _time; }
        bool operator<(const SimEvent& r) const { return _time > r._time; } // we want this sorted the other way
        bool operator==(const SimEvent& r) const { return _event == r._event; }
    };
       // std::priority_queue<SimEvent> _queue; // need some way to cancel it
        std::set<SimEvent> _queue;
        time_slice _sim_time;
         std::mutex _mutex;
         // this is so we can copy events on repeat and keep the same id
        void activate(const SimEvent& e, time_slice event_time);
    public:
        EventSystem();
        void incremtSimTime(time_slice i);
        time_slice simTime() const { return _sim_time; }
        void process_event();
        void activate (const EventFunction* func, time_slice event_time);
       // EventID activate (EventFunction& func, time_ms event_time) { return activate(func,std::)
    //    EventID activate (EventFunction& func, time_us event_time);
        void cancel(const EventFunction* func);
    };
}

#endif // PDP8EVENTS_H
