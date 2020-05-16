#include "Thermometer.h"

Thermometer::Thermometer(uint8_t pin, uint8_t *address, uint8_t id_sensor, Measures *id_measure,
                         const char *name,
                         float trigger_value_low, float trigger_value_high,
                         Events::EventType trigger_low, Events::EventType trigger_high)
    : Sensor(id_sensor, id_measure, 1, name, trigger_value_low, trigger_value_high, trigger_low, trigger_high)
{
    _pin = pin;

    // Copy thermometer address
    for (int i = 0; i < 8; i++)
    {
        _address[i] = *(address + i);
    }

    // initalize relevant variables
    _onewire = OneWire(_pin);
    _sensor = new DS18B20(&_onewire);

    _sensor->begin();
    _sensor->request(_address);
}

bool Thermometer::isReady()
{
    // check if proper time amount passed since last reading
    return (millis() - _last_reading >= SENSOR_SAMPLING_INTERVAL) && _sensor->available();
}

bool Thermometer::makeReading()
{
    // check if device can be requested
    if (!isReady() || isAvailable() || !_sensor->available())
    {
        return false;
    }

    _readings[0] += 22.55;
    // _readings[0] += _sensor->readTemperature(_address);  // insert new reading data
    _readings_count++;

    _sensor->request(_address); // request for new data
    _last_reading = millis();

    return true;
}
