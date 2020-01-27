/*
  WaterLevel.h - wraps HC-SR04 sensor to get water level
  Created by Adrian Kuboszek adrian.kuboszek@gmail.com 04.05.2019
  Last update: 
*/

#ifndef WaterLevel_h
#define  WaterLevel_h

#include "Sensor.h"

#include <Arduino.h>

#define WATER_LEVEL_UPPER_LEVEL 5.0
#define WATER_LEVEL_LOWER_LEVEL 15.0

enum WaterStatus {LACKING, OK, OVERFILLED, FILLING, PUMPING_OUT};

class WaterLevel: public Sensor
{
	public:
		WaterLevel(int echo, int trig, int id_sensor);
		bool makeReading();

		WaterStatus getWaterStatus();

	private:
		int _echo;
		int _trig;
};

#endif
