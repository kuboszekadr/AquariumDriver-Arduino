#ifndef Events_h
#define Events_h

#define EVENT_QUEUE_LENGTH 10
#define EVENT_MAX_SUBSCRIBERS 3

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

class Event
{
public:
    Event(EventType event);

    static void subscribe(EventType event);

private:
    static Event *_events[5];

    EventType _type;

    EventSubscriber _subscribers[EVENT_MAX_SUBSCRIBERS];
    static void _new_subscriber(EventType event);
    int _subscribers_amount;
};

void pushEvent(EventType event);

} // namespace Events

#endif