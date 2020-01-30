#include "WaterLevel.h"

WaterLevel::WaterLevel(int echo, int trig, int id_sensor)
{
	_echo = echo; // echo pin
	_trig = trig; // trig pin
	_id_sensor = id_sensor;

	pinMode(_trig, OUTPUT);
	pinMode(_echo, INPUT);
}

// WaterStatus WaterLevel::getWaterStatus()
// {
// 	// push(WATER_LOW);
// 	// push(WATER_HIGH);
// 	if (_last_reading_value < WATER_LEVEL_UPPER_LEVEL) // too much water
// 		return OVERFILLED;
// 	else if (_last_reading_value > WATER_LEVEL_LOWER_LEVEL) // water is lacking
// 		return LACKING;
// 	else
// 		return OK;
// }

bool WaterLevel::makeReading()
{
	//  check if sensor is ready or if data is full
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

Event WaterLevel::checkState()
{
	Event event = Events::NONE;
	if (_readings[_readings_count] >= WATER_LEVEL_UPPER_LEVEL)
	{
		event = Events::WATER_HIGH;
	}
	else if (_readings[_readings_count <= WATER_LEVEL_LOWER_LEVEL])
	{
		event = Events::WATER_HIGH;
	}

	if (event != Events::NONE)
	{
		Events::pushEvent(event);
	}
}