#include "Programs.h"

Programs::Program::Program(uint8_t relay_pin, uint8_t id, const Events::EventType *events_subscription, uint8_t events)
{
    _relay = new Relay(relay_pin);
    _id = id;

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

    addToI2CBuffer(1);
    _relay->turnOn();
}

void Programs::Program::end()
{
    addToI2CBuffer(0, 0);
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

void Programs::Program::addToI2CBuffer(uint8_t active, uint8_t step=1)
{
    char timestamp[17] = {};
    RTC::getTimestamp(timestamp);

    char json[100] = {};
    sprintf_P(json, PSTR("{\"pid\":%d,\"a\":%d,\"s\":%d,\"t\":%s}"), _id, active, step, timestamp);
    
    i2c::addToBuffer(json);
    Serial.println(json);
}