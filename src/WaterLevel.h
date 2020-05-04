/*
  WaterLevel.h - wraps HC-SR04 sensor to get water level
  Created by Adrian Kuboszek adrian.kuboszek@gmail.com 04.05.2019
  Last update: 
*/

#ifndef WaterLevel_h
#define WaterLevel_h

#include "Sensor.h"
#include <Arduino.h>

class WaterLevel : public Sensor
{
public:
	WaterLevel(int echo, int trig, int id_sensor, int id_measure,
			   const char *name,
			   float trigger_value_low, float trigger_value_high,
			   Events::EventType trigger_low, Events::EventType trigger_high);
	bool makeReading();
	Events::EventType checkTriggers();

private:
	int _echo;
	int _trig;
};

#endif
