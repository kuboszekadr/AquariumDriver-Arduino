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

void Programs::Program::addToI2CBuffer(const uint8_t active, const uint8_t step=1)
{
    char timestamp[20];
    RTC::getTimestamp(timestamp);

    char json[60];
    // Prepare JSON to be exported later on:
    // Codes:
    // id - program_id
    // e - execution_id
    // a - is active
    // s - step
    // t - timestamp in YYYYMMDD HHmmSS format
    // Format: {"s":1,"a":1,"s":0,"t":20200513 063312}
    // using single chars save a lot in buffer
 
    // sprintf_P(json, 
    //     PSTR("{\"id\":%d,\"e\":%d,\"a\":%d,\"s\":%d,\"t\":%s,\"c\":%d}"),
    //         _id, _execution_id, active, step, timestamp, PROGRAM_JSON_CLASS);
 
    // Above not working, walk around method
    sprintf_P(json, 
        PSTR("{\"id\":%d,\"e\":%d,\"a\":%d,\"s\":%d,\"t\":"),
            _id, _execution_id, active, step);
    strcat(json, timestamp);
    strcat(json, ",\"c\":");

    char _class[3];
    itoa(PROGRAM_JSON_CLASS, _class, 10);

    strcat(json, _class);
    strcat(json, "}");
 
    i2c::addToBuffer(json);
}