#include "pdp8events.h"
#include "pdp8i.h"

namespace PDP8 {
//static std::mutex _mutex;
EventSystem::EventSystem() :_sim_time(0){}
// this eliminates the need for update_sim_time
void EventSystem::incremtSimTime(time_slice i) {
    _sim_time+=i; // do something if we turn this over
}

void EventSystem::process_event() {
    _mutex.lock();
    std::set<SimEvent>::iterator i;

    auto it = _queue.begin();
    while(it != _queue.end() && _sim_time < it->time()) {
        const SimEvent& evt = *it;
        _mutex.unlock(); // we unlock, in case we are changing the sim, like cancling another event
        time_slice next_time = evt.execute();
        _mutex.lock();
        if(next_time!= time_slice::zero()) {
            SimEvent repeat(evt,next_time+_sim_time);
            _queue.erase(it++);
             _queue.insert(repeat);
        } else _queue.erase(it++);
    }
     _mutex.unlock();
}
void EventSystem::activate (const SimEvent& e, time_slice event_time) {
  //SimEvent event(e,event_time+_sim_time);
  _queue.emplace(e,event_time+_sim_time);
}

void EventSystem::activate(const EventFunction* func, time_slice event_time) {
     std::lock_guard<std::mutex> lg(_mutex);
    _queue.emplace(func,event_time+_sim_time);
}


void EventSystem::cancel(const EventFunction* id) {
    std::lock_guard<std::mutex> lg(_mutex);
    SimEvent searchfor(id,_sim_time);// should be after this
    auto cancelit = _queue.find(searchfor);
    if(cancelit != _queue.end()) _queue.erase(cancelit);
}

}
