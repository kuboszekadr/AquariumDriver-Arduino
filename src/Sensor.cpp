#include "Sensor.h"

uint8_t Sensor::sensors_amount = 0;
Sensor *Sensor::sensors[SENSOR_AMOUNT];

Sensor::Sensor(uint8_t id_sensor,
               Measures *id_measures,
               uint8_t measures,
               const char *name,
               float trigger_value_low, float trigger_value_high,
               Events::EventType trigger_low, Events::EventType trigger_high)
{
    if (sensors_amount == SENSOR_AMOUNT)
    {
        return;
    }

    sensors[sensors_amount] = this;                            // add sensor to the list of sensors
    sensors_amount += sensors_amount == SENSOR_AMOUNT ? 0 : 1; // increase amount of sensors

    _measures_amount = measures;
    _readings = new float[_measures_amount]();
    _last_readings = new float[_measures_amount]();
    _id_measures = new uint8_t[_measures_amount]();

    // convert measures enum into uint8_t
    for (uint8_t i = 0; i < _measures_amount; i++)
    {
        _id_measures[i] = static_cast<uint8_t>(id_measures[i]);
    }

    strcpy(_name, name); // copy sensor name
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
    reading.id_sensor = _id_sensor;             // sensor id in database
    reading.id_measures = _id_measures;         // measures get from the sensor
    reading.measures_amount = _measures_amount; // technical to be able to loop through measures

    // calcualte average value over time
    for (uint8_t i = 0; i < _measures_amount; i++)
    {
        _readings[i] /= _readings_count;  // calculate averages
        _last_readings[i] = _readings[i]; // save values
        _readings[i] = 0.0;               // prepare for new data
    }

    reading.values = _last_readings; // copy it into reading struct
    _readings_count = 0;             // restart counter

    return reading;
}

void Sensor::setTriggers(float trigger_value_low = -1.0, float trigger_value_high = -1.0)
{
    if (trigger_value_low > 0)
    {
        _trigger_value_low = trigger_value_low;
    }

    if (trigger_value_high > 0)
    {
        _trigger_value_high = trigger_value_high;
    }        
}

float Sensor::getTriggerValue(bool low)
{
    return low ? _trigger_low : _trigger_high;
}

bool Sensor::isAvailable()
{
    // check if sensor collected enough data
    return _readings_count >= SENSOR_SAMPLING_AMOUNT;
}

bool Sensor::isReady()
{
    // check if proper time amount passed since last reading
    return (millis() - _last_reading >= SENSOR_SAMPLING_INTERVAL);
}

char *Sensor::getName()
{
    return _name;
}

float *Sensor::getReadings()
{
    return _last_readings;
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