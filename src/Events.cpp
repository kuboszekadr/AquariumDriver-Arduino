#include "Events.h"

Event::Event(EventType event)
{
    _type = event;
    _events[event] = this;
}

void Events::pushEvent(EventType event)
{
    if (queue_length >= EVENT_QUEUE_LENGTH)
    {
        return;
    }

    EventsQueue[queue_length++] = Event;
}

void Event::subscribe(EventType event)
{
    // check if event has been already declarated
    if (!_events[event]) // note that each event has it's own place in the array
    {
        Event _event = new Event(event); // create new Event instance
        _events[event] = &_event;        // pass pointer of it into the events array
    }

    _new_subscriber(EventType event);  // add subscriber
}

void Event::_new_subscriber(EventType event)
{
    Event *_event = _events[event];  // get event instance
    _event->_subscribers_amount++;  // add subscriber
}