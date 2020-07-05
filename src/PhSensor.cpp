#include "PhSensor.h"

Sensor::PhSensor::PhSensor(uint8_t pin, uint8_t id_sensor, Measures *id_measure,
						   const char *name,
						   float trigger_value_low, float trigger_value_high,
						   Events::EventType trigger_low, Events::EventType trigger_high)
	: Sensor(id_sensor, id_measure, 1, name, trigger_value_low, trigger_value_high, trigger_low, trigger_high)
{
	_pin = pin;
}

bool Sensor::PhSensor::makeReading()
{
	//  check if sensor is ready or if data is full
	if (!isReady() || isAvailable())
		return false;

	float voltage = analogRead(_pin); // read voltage from the analog pin
	float ph = voltage * 5.0 / 1024;  // translate voltage into Ph

	_readings[0] += 7.0;
	_readings_count++;

	_last_reading = millis();
	return true;
}

Events::EventType Sensor::PhSensor::checkTriggers()
{
	Events::EventType event = Events::EventType::EMPTY;
	return event;
}
