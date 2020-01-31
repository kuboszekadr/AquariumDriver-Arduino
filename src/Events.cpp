#include "Events.h"

using namespace Events;

// int queueLength = 0;
EventType EventQueue[EVENT_QUEUE_LENGTH];

void EventSubscriber::subscribe(EventType event)
{
    // check if event has been already declarated
    if (!Event::_events[event]) // note that each event has it's own place in the array
    {
        Event *_event = new Event(event); // create new Event instance
        Event::_events[event] = _event; // pass pointer of it into the events array
    }

    Event::_new_subscriber(event, this); // add subscriber
}

Event::Event(EventType event)
{
    _type = event;
    _events[event] = this;
}

void pushEvent(EventType event)
{
    if (queueLength >= EVENT_QUEUE_LENGTH)
    {
        return;
    }

    EventsQueue[queueLength++] = event;
}

void Event::_new_subscriber(EventType event, EventSubscriber *subscriber)
{
    Event *_event = _events[event]; // get event instance
    _event->_subscribers[0] = subscriber;
}
