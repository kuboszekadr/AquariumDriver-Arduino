#include "Events.h"

int Events::queueLength = 0;
Events::EventType Events::EventsQueue[EVENT_QUEUE_LENGTH];
Events::Event *Events::Event::_events[7];

void Events::EventSubscriber::subscribe(EventType event)
{
    // check if event has been already declarated
    // note that each event has it's own place in the array
    if (!Event::_events[event])
    {
        new Event(event); //create new event object on the heap
    }

    Event::_new_subscriber(event, this); // add event subscriber
}

Events::Event::Event(EventType event_type)
{
    // TODO: check if event has been init
    _events[event_type] = this; // place event pointer in the array of events

    _type = event_type; // assing event type
}

void Events::Event::_new_subscriber(EventType event, EventSubscriber *subscriber)
{
    Event *_event = _events[event];                // get event instance
    int subscribers = _event->_subscribers_amount; // get current subscribers _subscribers_amount

    // add new subscriber
    _event->_subscribers[subscribers] = subscriber; // add subscriber to the list of subscribers
    _event->_subscribers_amount = ++subscribers;
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
    if (queueLength >= EVENT_QUEUE_LENGTH)
    {
        Logger::log(F("Events queue is full"), LogLevel::WARNING);
        return;
    }
    else if (event == EMPTY)
    {
        return;
    }

    EventsQueue[queueLength++] = event; // insert event into queue
}

void Events::notifySubscribers()
{
    // loop through subscribers and notify about event
    for (int i = 0; i < queueLength; i++)
    {
        EventType event_type = EventsQueue[i];      // take event type from the queue
        Event *event = Event::getEvent(event_type); // get event object based on event type

        if (event)
        {
            event->notifySubscribers();
        }
        else
        {
            Logger::log(F("Event has no subscribers"), LogLevel::WARNING);
        }
        EventsQueue[i] = EventType::EMPTY;
    }
    queueLength = 0; // restart queue
}