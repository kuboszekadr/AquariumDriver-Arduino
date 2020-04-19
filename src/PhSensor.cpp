#include "PhSensor.h"

PhSensor::PhSensor(int pin, int id_sensor,
				   const char *name,
				   float trigger_value_low, float trigger_value_high,
				   Events::EventType trigger_low, Events::EventType trigger_high)
	: Sensor(id_sensor, name, trigger_value_low, trigger_value_high, trigger_low, trigger_high)
{
	_pin = pin;
}

bool PhSensor::make_reading()
{
	//  check if sensor is ready or if data is full
	if (!isReady() || isAvailable())
		return false;

	float voltage = analogRead(_pin); // read voltage from the analog pin
	float ph = voltage * 5.0 / 1024;  // translate voltage into Ph

	// count how many readings are available in the array
	_readings_count = (_readings_count + 1) % SENSOR_SAMPLING_AMOUNT; // for safety reasons
	_readings[_readings_count] = 7.0;
	// _readings[_readings_count] = ph;								  // insert new reading data

	_last_reading = millis();
	return true;
}

Events::EventType PhSensor::checkTriggers()
{
	Events::EventType event = Events::EventType::EMPTY;
	return event;
}
