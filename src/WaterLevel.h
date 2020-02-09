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
	WaterLevel(int echo, int trig, int id_sensor);
	bool makeReading();

private:
	int _echo;
	int _trig;
	int _i;
};

#endif
