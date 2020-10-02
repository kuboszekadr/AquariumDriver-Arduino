#include "Thermometer.h"

Sensor::Thermometer::Thermometer(
    uint8_t pin,
    uint8_t *address,
    uint8_t id_sensor,
    Measures *id_measure,
    const char *name,

    uint32_t sampling_interval,
    uint8_t sampling_amount,
    float trigger_value_low,
    float trigger_value_high,


    Events::EventType trigger_low,
    Events::EventType trigger_high)
    : Sensor(
          id_sensor,
          id_measure,
          1,
          name,

          sampling_interval,
          sampling_amount,

          trigger_value_low,
          trigger_value_high,

          trigger_low,
          trigger_high)
{
    _pin = pin;

    // Copy thermometer address
    memcpy(_address, address, 8);

    // initalize relevant variables
    _onewire = OneWire(_pin);
    _sensor = new DS18B20(&_onewire);

    _sensor->begin();
    _sensor->request(_address);
}

bool Sensor::Thermometer::isReady()
{
    // check if proper time amount passed since last reading
    return (millis() - _last_reading >= _sampling_interval) && _sensor->available();
}

bool Sensor::Thermometer::makeReading()
{
    // check if device can be requested
    if (!isReady() || isAvailable() || !_sensor->available())
    {
        return false;
    }

    _readings[0] += _sensor->readTemperature(_address); // insert new reading data
    _readings_count++;

    _sensor->request(_address); // request for new data
    _last_reading = millis();

    return true;
}
