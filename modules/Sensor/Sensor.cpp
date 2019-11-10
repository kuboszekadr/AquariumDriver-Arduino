#include "Sensor.h"

bool Sensor::available()
{
    // check if array is full
    return (_readings_count + 1) == SAMPLING_AMOUNT;
}

Reading Sensor::get_reading()
{
	// create reading data
	struct Reading reading;
	reading.id_sensor = _id_sensor;  // sensor id in database
	reading.value = _avg();  // average over sampling time
	RTC::get_timestamp(reading.timestamp);  // time of sampling taking

    _readings_count = -1; // restart counter
    _last_reading_value = reading.value;  // save last reading

	return reading;
}

bool Sensor::ready()
{
    // check if proper time amount passed since last reading
    return (abs(millis() - _last_reading) >= SAMPLING_INTERVAL);
}

float Sensor::_avg()
{
    float _avg = avg(_readings, SAMPLING_AMOUNT);  // get average of readings 
    memset(_readings, 0, SAMPLING_AMOUNT);  // clear array content
    
    _readings_count = -1;  // restart counter after data sending
    return _avg;  // return average of readings
}

void Sensor::attach_relay(int pin)
{
    relay = new Relay(pin);
}