#include "pdp8events.h"
#include "pdp8i.h"

namespace PDP8 {
 void Bus::busThread() { // main thread the bus runs on that picks up messages from devices

     class BusEventContainer {
         BusEvent* _event;
         uint64_t _delay;
     public:
         BusEventContainer(BusEvent* event) : _event(event) , _delay(event->interval()) {}
         BusEventContainer(BusEvent* event, uint64_t delay) : _event(event) , _delay(delay) {}
         BusEvent* event() const { return _event; }
         uint64_t delay() const { return _delay; }
         void delay(uint64_t delay) { _delay = delay; }
         bool operator<(const BusEventContainer& r) const { return _delay < r._delay; }
         bool operator==(const BusEventContainer& r) const { return _event == r._event; }
     };
    std::set<BusEventContainer> events;
    std::unique_lock<std::mutex> lk(_mutex);
    _lastReply = BusMessageReply::Idle;
    _message = BusMessage::Idle;

    do {
        switch(_message) {
        case BusMessage::AddToBus: // we add the device to the bus
            _lastReply = BusMessageReply::Error;
            if(_operand || _operand->_bus==nullptr){
                _operand->_bus = this;
                _countdown = _operand->interval();
                events.emplace(_operand);
                _lastReply = BusMessageReply::NoError;
                _countdown = events.begin()->delay();
            }
            _message = BusMessage::Idle;
            _waitRespond.notify_one();
            break;
        case BusMessage::RemoveFromBuss:
            _lastReply = BusMessageReply::Error;
            if(_operand || _operand->_bus == this){
                auto it = events.find(BusEventContainer(_operand));
                if(it != events.end()) {
                    events.erase(it);
                    _operand->_bus = nullptr;
                    _lastReply = BusMessageReply::NoError;
                }
            }
            _message = BusMessage::Idle;
            _waitRespond.notify_one();
            break;
        case BusMessage::RunTopEvent:



        case BusMessage::Idle:
            _postMessage.wait(lk,[this](){ return _message != BusMessage::Idle;});
            break;
        default:
            _message = BusMessage::Idle;
            break;
        }
    }  while(_message != BusMessage::StopThread);
    _lastReply = BusMessageReply::ThreadStopped;
 }
Bus::Bus() : _lastReply(BusMessageReply::ThreadStopped), _message(BusMessage::Idle) {
    std::thread(&Bus::busThread,this).detach();
}


void Bus::postMessage(BusMessage msg,BusEvent* operand){
     std::lock_guard<std::mutex> lk(_mutex);
      _lastReply = BusMessageReply::Idle;
    _message = msg;
    _operand = operand;
    _postMessage.notify_one();
}

BusMessageReply Bus::postMessageWait(BusMessage msg,BusEvent* operand){
    std::unique_lock<std::mutex> lk(_mutex);
   _lastReply = BusMessageReply::Idle;
   _message = msg;
   _operand = operand;
   _postMessage.notify_one();
   _waitRespond.wait(lk,[this]() { return _lastReply != BusMessageReply::Idle; });
   BusMessage lastReply = _lastReply;
   _lastReply = BusMessage::Idle;
   return lastReply;
}



void BusEvent::addToBus(Bus* bus){


}
void BusEvent::removeFromBus(){

}

bool BusEvent::start(){

}

bool BusEvent::stop(){

}

bool BusEvent::pause(){

}

  //  uint64_t _interval;
  //  bool _restart;
  //  bool _isRunning; // only availabe in Bus


}
