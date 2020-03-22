#include "Thermometer.h"

Thermometer::Thermometer(int pin, uint8_t *address, int id_sensor,
                         const char *name,
                         float trigger_value_low, float trigger_value_high,
                         Events::EventType trigger_low, Events::EventType trigger_high)
    : Sensor(id_sensor, name, trigger_value_low, trigger_value_high, trigger_low, trigger_high)
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
        return false;

    // count how many readings are available in the array
    _readings_count = (_readings_count + 1) % SENSOR_SAMPLING_AMOUNT; // for safety reasons
    _readings[_readings_count] = _sensor->readTemperature(_address);  // insert new reading data

    _sensor->request(_address); // request for new data

    _last_reading = millis();
    return true;
}
