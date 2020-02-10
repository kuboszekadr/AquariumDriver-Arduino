#include "Events.h"

int Events::queueLength = 0;
Events::EventType Events::EventsQueue[EVENT_QUEUE_LENGTH];
Events::Event *Events::Event::_events[5];

void Events::EventSubscriber::subscribe(EventType event)
{
    // check if event has been already declarated
    // note that each event has it's own place in the array
    if (!Event::_events[event])
    {
        Event _event = Event(event);
    }

    Event::_new_subscriber(event, this); // add event subscriber
}

Events::Event::Event(EventType event_type)
{
    _type = event_type; // assing event type
    // TODO: check if event has been init
    _events[event_type] = this; // place event pointer in the array of events
}

void Events::Event::_new_subscriber(EventType event, EventSubscriber *subscriber)
{
    Event *_event = _events[event];       // get event instance
    _event->_subscribers[0] = subscriber; // add subscriber to the list of subscribers
}

void Events::Event::notifySubscribers()
{
    // loop through all event subscribes
    for (int i = 0; i < _subscribers_amount; i++)
    {
        _subscribers[i]->reactForEvent(_type);
    }
}

Events::Event *Events::Event::getEvent(EventType event)
{
    return _events[event];
}

void Events::raise(EventType event)
{
    // if queue is full or event is empty ommit
    if (queueLength >= EVENT_QUEUE_LENGTH || event == EMPTY)
    {
        return;
    }

    EventsQueue[queueLength++] = event; // insert event into queue
}

void Events::notifySubscribers()
{
    int i = 0;

    // loop through subscribers and notify about event
    for (int i = 0; i < Events::queueLength; i++)
    {
        Events::EventType event_type = Events::EventsQueue[i];

        Events::Event *event = Event::getEvent(event_type);
        event->notifySubscribers();

        EventsQueue[i] = EventType::EMPTY;
    }

    Events::queueLength = 0; // restart queue
}