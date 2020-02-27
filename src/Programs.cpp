#include "Programs.h"

Programs::Program::Program(int relay_pin, const Events::EventType *events_subscription, int events)
{
    _relay = new Relay(relay_pin);

    Events::EventType *event = events_subscription;
    for (int i = 0; i < events; i++)
    {
        subscribe(*(event + i));
    }
}

void Programs::Program::start()
{
    if (_is_active)
    {
        return;
    }

    _relay->turnOn();
}

void Programs::Program::end()
{
    _relay->turnOff();
}

bool Programs::Program::isActive()
{
    return _is_active;
}

void Programs::Program::reactForEvent(Events::EventType event)
{
    if (_state == event)
    {
        return;
    }

    // TODO
}