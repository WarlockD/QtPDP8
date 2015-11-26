#ifndef PDP8EVENTS_H
#define PDP8EVENTS_H
#include "pdp8state.h"


// so we are making an event queue, its a thing
// The purpose of the events queue is to act like the bus
// diffrent devices send messages to here that sync with the cpu
// so in essence this acts like the bus that operates independently of the
// CPU and devices


namespace PDP8 {
    enum class BusMessage : uint8_t {
        Idle,
        AddToBus,
        RemoveFromBuss,
        StartEvent,
        StopEvent,
        PauseEvent,
        // Internal stuff
        RunTopEvent,
        StopThread,

    };
    enum class BusMessageReply : uint8_t {
        // Status
        Idle,
        ThreadStopped,
        // Replys
        NoError,
        Working,
        Error,
    };

    class Bus {

        std::mutex _mutex;
        std::condition_variable _postMessage;
        std::condition_variable _waitRespond;
        BusMessage _message;
        BusEvent* _operand;
        BusMessageReply _lastReply;
        uint64_t _countdown;
        void busThread(); // main thread the bus runs on that picks up messages from devices
    public:
        Bus();
        void postMessage(BusMessage msg,BusEvent* operand=nullptr);
        BusMessageReply postMessageWait(BusMessage msg,BusEvent* operand=nullptr);
        bool tickCountdown(); // we run a tick, or one phase of the cpu, if it
    };
}

class pdp8events
{
public:
    pdp8events();

signals:

public slots:
};

#endif // PDP8EVENTS_H
