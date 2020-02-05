#include "Events.h"

int Events::queueLength = 0;
Events::EventType Events::EventsQueue[EVENT_QUEUE_LENGTH];
Events::Event *Events::Event::_events[5];

void Events::EventSubscriber::subscribe(EventType event)
{
    // // check if event has been already declarated
    if (!Event::_events[event]) // note that each event has it's own place in the array
    {
        Event _event = Event(event);
    }

    Event::_new_subscriber(event, this); // add subscriber
}

Events::Event::Event(EventType event_type)
{
    _type = event_type;         // assing event type
    _events[event_type] = this; // place event pointer in the array of events
    // TODO: check if event has been init
}

void Events::Event::_new_subscriber(EventType event, EventSubscriber *subscriber)
{
    Event *_event = _events[event];       // get event instance
    _event->_subscribers[0] = subscriber; // add subscriber to the list of subscribers
}

void Events::Event::notifySubscribers()
{
    // loop through all event subsribes
    for (int i = 0; i < _subscribers_amount; i++)
    {
        _subscribers[i]->reactForEvent();
    }
}

Events::Event *Events::Event::getEvent(EventType event)
{
    return _events[event];
}

void Events::pushEvent(EventType event)
{
    Serial.print("New event: \t");
    Serial.println(event);

    if (queueLength >= EVENT_QUEUE_LENGTH)
    {
        return;
    }

    EventsQueue[queueLength++] = event;
}

void Events::notifySubscribers()
{
    Serial.println("Notifying...");
    int i = 0;

    for (int i = 0; i < Events::queueLength; i++)
    {
        Serial.println(Events::EventsQueue[i]);
        Events::EventType event_type = Events::EventsQueue[i];

        Events::Event *event = Event::getEvent(event_type);
        event->notifySubscribers();
    }

    Events::queueLength = 0;  // restart queue
}