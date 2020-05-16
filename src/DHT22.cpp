#include "DHT22.h"

DHT22::DHT22(uint8_t pin, uint8_t id_sensor, Measures *id_measures,
          const char *name,
          float trigger_value_low, float trigger_value_high,
          Events::EventType trigger_low, Events::EventType trigger_high)
    : Sensor(id_sensor, id_measures, 2, name, trigger_value_low, trigger_value_high, trigger_low, trigger_high)
{
    _pin = pin;
}

bool DHT22::makeReading()
{
	//  check if sensor is ready or if data is full
	if (!isReady() || isAvailable())
		return false;

    _readings[0] += 70.00;
	_readings[1] += 22.20;
	_readings_count++;

	_last_reading = millis();
	return true;

}