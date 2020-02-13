#include "WaterLevel.h"

WaterLevel::WaterLevel(int echo, int trig, int id_sensor, float trigger_low, float trigger_high)
	: Sensor(id_sensor, trigger_low, trigger_high)
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

Events::EventType WaterLevel::checkTriggers()
{
	Events::EventType event = Events::EventType::EMPTY;

	// check current level of water
	if (_last_reading_value < _trigger_low)
	{
		event = Events::EventType::WATER_LOW;
	}
	else if (_last_reading_value > _trigger_high)
	{
		event = Events::EventType::WATER_HIGH;
	}
	
	// push to the queue if event is not empty
	if (event != Events::EventType::EMPTY)
	{
		Events::raise(event);
	}

	return event;
}