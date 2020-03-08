#ifndef Events_h
#define Events_h

#define EVENT_QUEUE_LENGTH 10
#define EVENT_MAX_SUBSCRIBERS 3

#include "Log.h"

#include <Arduino.h>

namespace Events
{
enum EventType
{
    EMPTY = 0,
    RELAY_STATE_CHANGE,

    WATER_LOW,
    WATER_HIGH,

    TEMP_LOW,
    TEMP_HIGH

    // TODO:
    // CO2_LOW,
    // CO2_HIGH
};

class EventSubscriber
{
public:
    void subscribe(EventType event);
    virtual void reactForEvent(EventType event) = 0;
};

class Event
{
public:
    Event(EventType event_type);
    void notifySubscribers();

    static Event *getEvent(EventType event);

private:
    static Event *_events[7] = {};  //event object list
    static void _new_subscriber(EventType event, EventSubscriber *subscriber);

    EventType _type;

    EventSubscriber *_subscribers[EVENT_MAX_SUBSCRIBERS];
    int _subscribers_amount = 0;

    friend EventSubscriber;
};

void raise(EventType event);
void notifySubscribers();

extern EventType EventsQueue[EVENT_QUEUE_LENGTH];
extern int queueLength;

const char EventTypeLabels[7][21] = {"EMPTY", "RELAY_STATE_CHANGE", "WATER_LOW", "WATER_HIGH", "TEMP_LOW", "TEMP_HIGH"};

}; // namespace Events

#endif