#ifndef Events_h
#define Events_h

#define EVENT_QUEUE_LENGTH 10
#define EVENT_MAX_SUBSCRIBERS 3

#include <Arduino.h>

namespace Events
{
enum EventType
{
    RELAY_STATE_CHANGE,

    WATER_LOW,
    WATER_HIGH,

    TEMP_LOW,
    TEMP_HIGH
};

class EventSubscriber
{
public:
    EventSubscriber() {};
    void subscribe(EventType event);
    virtual void reactForEvent(EventType event)=0;
};

class Event
{
public:
    Event(EventType event_type);
    void notifySubscribers();

    static Event *getEvent(EventType event);

private:
    static Event *_events[5];
    static void _new_subscriber(EventType event, EventSubscriber *subscriber);

    EventType _type;

    EventSubscriber *_subscribers[EVENT_MAX_SUBSCRIBERS];
    int _subscribers_amount;

    friend EventSubscriber;
};

void raise(EventType event);
void notifySubscribers();

extern EventType EventsQueue[EVENT_QUEUE_LENGTH];
extern int queueLength;

}; // namespace Events

#endif