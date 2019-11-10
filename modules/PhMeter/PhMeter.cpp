#include "PhMeter.h"

PhMeter::PhMeter(int pin, int id_sensor)
{
    _pin = pin;
    _id_sensor = id_sensor;
}

bool PhMeter::make_reading()
{
	//  check if sensor is ready or if data is full
	if (!ready() || available()) 
		return false;

    float voltage = analogRead(_pin);  // read voltage from the analog pin
    float ph = voltage*5.0/1024;  // translate voltage into Ph

	// count how many readings are available in the array
	_readings_count = (_readings_count + 1) % SAMPLING_AMOUNT;  // for safety reasons
	_readings[_readings_count] = ph; // insert new reading data

	_last_reading = millis();
	return true;
}