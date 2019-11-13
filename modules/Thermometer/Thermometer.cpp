#include "Thermometer.h"

Thermometer::Thermometer(int pin, int id_sensor, uint8_t *address)
{
    _pin = pin;
    _id_sensor = id_sensor;

    memcpy(_address, address, 8);
    // _address = address;

    _onewire = OneWire(pin);
    _sensor = new DS18B20(&_onewire);   

    _sensor->begin(12);
    _sensor->request(_address);
}

bool Thermometer::ready()
{
    // check if proper time amount passed since last reading
    return (abs(millis() - _last_reading) >= SAMPLING_INTERVAL) && _sensor->available();
}

bool Thermometer::make_reading()
{
    if (!ready() || available() || !_sensor->available()) 
        return false;

	// count how many readings are available in the array
	_readings_count = (_readings_count + 1) % SAMPLING_AMOUNT;  // for safety reasons
	_readings[_readings_count] = _sensor->readTemperature(_address); // insert new reading data

    _sensor->request(_address);  // request for new data
	_last_reading = millis();
	return true;

}