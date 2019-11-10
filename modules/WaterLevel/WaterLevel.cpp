#include "WaterLevel.h"

WaterLevel::WaterLevel(int echo, int trig, int id_sensor)
{
	_echo = echo;  // echo pin
	_trig = trig;  // trig pin
	_id_sensor = id_sensor;
	
	pinMode(_trig, OUTPUT);
	pinMode(_echo, INPUT);
}

WaterStatus WaterLevel::get_water_status()
{
	if (_last_reading_value < UPPER_LEVEL)  // too much water
		return OVERFILLED;  
	else if (_last_reading_value > LOWER_LEVEL)  // water is lacking
		return LACKING; 
	else
		return OK;
}

bool WaterLevel::make_reading()
{
	//  check if sensor is ready or if data is full
	if (!ready() || available()) 
		return false;

	// returns water level in cm
	digitalWrite(_trig, LOW);
	delayMicroseconds(2);
	
	digitalWrite(_trig, HIGH);
	delayMicroseconds(10);
	
	// Measuring distance
	digitalWrite(_trig, LOW);

	// count how many readings are available in the array
	_readings_count = (_readings_count + 1) % SAMPLING_AMOUNT;  // for safety reasons
	_readings[_readings_count] = pulseIn(_echo, HIGH) / 58.0; // insert new reading data

	_last_reading = millis();
	return true;
}
