#include "WaterLevel.h"

Sensor::WaterLevel::WaterLevel(
	uint8_t echo,
	uint8_t trig,
	uint8_t id_sensor,
	Measures *id_measure,
	const char *name,

	float trigger_value_low,
	float trigger_value_high,

	uint32_t sampling_interval,
	uint8_t sampling_amount,

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
	_echo = echo; // echo pin
	_trig = trig; // trig pin

	pinMode(_trig, OUTPUT);
	pinMode(_echo, INPUT);
}

bool Sensor::WaterLevel::makeReading()
{
	digitalWrite(_trig, LOW);
	delayMicroseconds(2);

	digitalWrite(_trig, HIGH);
	delayMicroseconds(10);

	// Measuring distance
	digitalWrite(_trig, LOW);

	// Get value
	_readings[0] += pulseIn(_echo, HIGH) / 58.0; // returns water level in cm
	_readings_count++;

	_last_reading = millis();
	return true;
}

Events::EventType Sensor::WaterLevel::checkTrigger()
{
	Events::EventType event = Events::EventType::EMPTY;
	// check current level of water
	if (_last_readings[0] > _trigger_value_low)
	{
		event = _trigger_low;
	}
	else if (_last_readings[0] < _trigger_value_high)
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