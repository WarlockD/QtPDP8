#include "timer.h"

void TimerQueue::schedule(Timer* t, int64_t d) {
        if (t->_delay >= 0) { /* timer is already scheduled */
            /* don't mess up the queue, schedule becomes no-op */
        } else if (_head == nullptr) { /* nothing is scheduled yet */
            _countdown = d;
            t->_delay = d;
            t->_next = nullptr;
            _head = t;
        } else { /* there is an existing schedule */
            if (_countdown > d) {    /* t goes before old head */
                _head->_delay = _countdown - d;
                _countdown = d;    /* set timer until t */
                t->_next = _head;   /* link t ahead of old head */
                t->_delay = d;     /* mark timer as queued */
                _head = t;	  /* reset list head */
            } else { /* new event goes down into queue */
                Timer * i= _head;
                d = d - _countdown; /* delay relative to head */
                while (i->_next != nullptr) { /* scan list for place */
                    if (d < i->_next->_delay) {
                        i->_next->_delay = i->_next->_delay - d;
                        break;
                    }
                    i = i->_next;
                    d = d - i->_delay; /* adjust delay */
                }
                t->_next = i->_next; /* link new timer into queue */
                t->_delay = d;
                i->_next = t;
            }
        }
}
int64_t TimerQueue::query(const Timer* t) const  {
    if (t->_delay < 0) { /* timer is not in queue */
        return -1;
    } else {
        long int query = _countdown;
        Timer * i = _head;
        while (i != t) { /* scan list for t */
            i = i->_next;
            query = query + i->_delay; /* accumulate delays */
        }
        return query;
    }
}
void TimerQueue::fire_timer() {
    if (_head == nullptr) { /* no pending timer */
            /* put off next firing as long as possible */
            _countdown = INT64_MAX;
        } else {
            Timer* a = _head; // copy it
            /* mark head timer as idle */
            _head->_delay = INT64_MAX;

            _head = _head->_next;
            if (_head == nullptr) { /* no new timer */
                /* put off next firing as long as possible */
                _countdown = INT64_MAX;
            } else {
                /* figure delay till next timer */
                _countdown = _countdown + _head->_delay;
                /* note: this could have been countdown = head->delay,
                   except that countdown could have gone slightly
                   negative, and we want to make delays add nicely
                   for such devices as line frequency clocks */
            }

            /* fire action on head timer */
            a->runAction();
        }

}
