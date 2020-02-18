#include "Sensor.h"

unsigned int Sensor::sensors_amount = 0;
Sensor *Sensor::sensors[SENSOR_AMOUNT];

Sensor::Sensor(int id_sensor,
               float trigger_value_low, float trigger_value_high,
               Events::EventType trigger_low, Events::EventType trigger_high)
{
    sensors[sensors_amount] = this;                            // add sensor to the list of sensors
    sensors_amount += sensors_amount == SENSOR_AMOUNT ? 0 : 1; // increase amount of sensors

    _id_sensor = id_sensor;

    _trigger_value_low = trigger_value_low;
    _trigger_low = trigger_low;

    _trigger_value_high = trigger_value_high;
    _trigger_high = trigger_high;
}

Reading Sensor::getReading()
{
    // create reading data
    struct Reading reading;
    reading.id_sensor = _id_sensor; // sensor id in database
    reading.value = _avg();         // average over sampling time

    _readings_count = -1;                // restart counter
    _last_reading_value = reading.value; // save last reading

    return reading;
}

bool Sensor::isAvailable()
{
    // check if array is full
    return (_readings_count + 1) == SENSOR_SAMPLING_AMOUNT;
}

bool Sensor::isReady()
{
    // check if proper time amount passed since last reading
    return (abs(millis() - _last_reading) >= SENSOR_SAMPLING_INTERVAL);
}

float Sensor::_avg()
{
    float _avg = avg(_readings, SENSOR_SAMPLING_AMOUNT); // get average of readings
    memset(_readings, 0, SENSOR_SAMPLING_AMOUNT);        // clear array content

    _readings_count = -1; // restart counter after data sending
    return _avg;          // return average of readings
}

Events::EventType Sensor::checkTriggers()
{
    Events::EventType event = Events::EventType::EMPTY;
    // check current level of water
    if (_last_reading_value < _trigger_value_low)
    {
        event = _trigger_low;
    }
    else if (_last_reading_value > _trigger_value_high)
    {
        event = _trigger_high;
    }

    // push to the queue if event is not empty
    if (event != Events::EventType::EMPTY & event != _last_trigger)
    {
        Events::raise(event);
    }

    return event;
}