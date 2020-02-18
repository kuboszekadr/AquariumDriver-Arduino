#include "WaterLevel.h"

WaterLevel::WaterLevel(int echo, int trig, int id_sensor,
					   float trigger_value_low, float trigger_value_high,
					   Events::EventType trigger_low, Events::EventType trigger_high)
	: Sensor(id_sensor, trigger_value_low, trigger_value_high, trigger_low, trigger_high)
{
	_echo = echo; // echo pin
	_trig = trig; // trig pin

	pinMode(_trig, OUTPUT);
	pinMode(_echo, INPUT);
}

bool WaterLevel::makeReading()
{
	//  check if sensor is ready or if data array is full
	if (!isReady() || isAvailable())
	{
		return false;
	}

	// returns water level in cm
	digitalWrite(_trig, LOW);
	delayMicroseconds(2);

	digitalWrite(_trig, HIGH);
	delayMicroseconds(10);

	// Measuring distance
	digitalWrite(_trig, LOW);

	// count how many readings are available in the array
	_readings_count = (_readings_count + 1) % SENSOR_SAMPLING_AMOUNT; // for safety reasons
	_readings[_readings_count] = pulseIn(_echo, HIGH) / 58.0;		  // insert new reading data

	_last_reading = millis();
	return true;
}
