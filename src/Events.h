#ifndef Events_h
#define Events_h

#define EVENT_QUEUE_LENGTH 10
#define EVENT_MAX_SUBSCRIBERS 3
#define EVENT_DEFINED_EVENTS 9  // + 1 for defined event to have null terminator

#include "Log.h"

#include <Arduino.h>

// Definition of all event labels in PROGMEM
const char event_empty_str[] PROGMEM = "EMPTY";
const char event_relay_change_str[] PROGMEM = "RELAY_STATE_CHANGE";

const char event_water_low_str[] PROGMEM = "WATER_LOW";
const char event_water_high_str[] PROGMEM = "WATER_HIGH";

const char event_temp_low_str[] PROGMEM = "TEMP_LOW";
const char event_temp_high_str[] PROGMEM = "TEMP_HIGH";

const char event_ph_low_str[] PROGMEM = "PH_LOW";
const char event_ph_high_str[] PROGMEM = "PH_HIGH";

const char* const event_type_labels[] PROGMEM = {
    // other event types
    event_empty_str, 
    event_relay_change_str,
    
    // water level event names
    event_water_low_str,
    event_water_high_str,
    
    // temperature event names
    event_temp_low_str,
    event_temp_high_str,

    // ph event names
    event_ph_low_str,
    event_ph_high_str
};

namespace Events
{
enum EventType
{
    EMPTY = 0,
    RELAY_STATE_CHANGE,

    WATER_LOW,
    WATER_HIGH,

    TEMP_LOW,
    TEMP_HIGH,

    PH_LOW,
    PH_HIGH
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

    static Event *getEvent(EventType event) {return _events[event]; };

private:
    static Event *_events[EVENT_DEFINED_EVENTS] = {};  //event object list
    static void _new_subscriber(EventType event, EventSubscriber *subscriber);

    EventType _type;

    EventSubscriber *_subscribers[EVENT_MAX_SUBSCRIBERS];
    int _subscribers_amount = 0;

    friend EventSubscriber;
};

void raise(EventType event);
void notifySubscribers();
char *getEventLabel(EventType event);

extern EventType EventsQueue[EVENT_QUEUE_LENGTH];
extern int queue_length;

}; // namespace Events

#endif