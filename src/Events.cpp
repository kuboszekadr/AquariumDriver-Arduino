#include "Events.h"

int Events::queue_length = 0;
Events::EventType Events::EventsQueue[EVENT_QUEUE_LENGTH];
Events::Event *Events::Event::_events[EVENT_DEFINED_EVENTS];

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
    _type = event_type;         // assing event type
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

void Events::raise(EventType event)
{
    // if queue is full or event is empty ommit
    if (queue_length >= EVENT_QUEUE_LENGTH)
    {
        Logger::log(F("Events queue is full"), LogLevel::WARNING);
        return;
    }
    else if (event == EMPTY)
    {
        return;
    }

    EventsQueue[queue_length++] = event; // insert event into queue
}

void Events::notifySubscribers()
{
    // loop through subscribers and notify about event
    for (int i = 0; i < queue_length; i++)
    {
        EventType event_type = EventsQueue[i];      // take event type from the queue
        Event *event = Event::getEvent(event_type); // get event object based on event type

        if (event)
        {
            event->notifySubscribers();
        }
        else
        {
            char event_name[20];
            strcpy(event_name, getEventLabel(event_type));

            char msg[50];
            sprintf_P(msg, PSTR("Event %s has no subscribers"), event_name);

            Logger::log(msg, LogLevel::WARNING);
        }
        EventsQueue[i] = EventType::EMPTY;
    }
    queue_length = 0; // restart queue
}

char *Events::getEventLabel(EventType event)
{
    char buffer[21] = {};
    strncpy_P(buffer,
              (char *)pgm_read_word(&(event_type_labels[event])),
              20); // Necessary casts and dereferencing, just copy.
    return buffer;
}