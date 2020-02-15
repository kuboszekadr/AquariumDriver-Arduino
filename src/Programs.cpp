#include "Programs.h"

Programs::Program::Program(int relay_pin)
{
    // TODO: Add event subscribers
    _relay = new Relay(relay_pin);
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