#include "DHT22.h"

Sensor::DHT22::DHT22(
	uint8_t pin,
	uint8_t id_sensor,
	Measures *id_measures,
	const char *name,

	float trigger_value_low,
	float trigger_value_high,

	uint32_t sampling_interval,
	uint8_t sampling_amount,

	Events::EventType trigger_low,
	Events::EventType trigger_high)
	: Sensor(
		  id_sensor,
		  id_measures,
		  2, // readings amount
		  name,

		  sampling_interval,
		  sampling_amount,

		  trigger_value_low,
		  trigger_value_high,

		  trigger_low,
		  trigger_high),
	  DHT(pin, DHT_22)
{
	_pin = pin;
	begin();
}

bool Sensor::DHT22::makeReading()
{
	float humid = readHumidity();
	float temp = readTemperature();

	if (isnan(humid) || isnan(temp))
	{
		return false;
	}

	_readings[0] += temp;
	_readings[1] += humid;
	_readings_count++;

	_last_reading = millis();
	return true;
}