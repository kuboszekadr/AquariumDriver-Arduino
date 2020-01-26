#include "Sensor.h"

unsigned int Sensor::sensors_amount = 0;
Sensor* Sensor::sensors[SENSOR_AMOUNT];

Sensor::Sensor()
{
    sensors[sensors_amount] = this;  // add sensor to the list of sensors
    sensors_amount += sensors_amount == SENSOR_AMOUNT ? 0 : 1;  // increase amount of sensors
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

void Sensor::attachRelay(int pin)
{
    relay = new Relay(pin);
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
