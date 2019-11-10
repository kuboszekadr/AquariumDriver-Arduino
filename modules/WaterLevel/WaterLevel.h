/*
  WaterLevel.h - wraps HC-SR04 sensor to get water level
  Created by Adrian Kuboszek adrian.kuboszek@gmail.com 04.05.2019
  Last update: 
*/

#ifndef WaterLevel_h
#define  WaterLevel_h

#include "Arduino.h"
#include "Utils.h"
#include "Sensor.h"
#include "RTC.h"

#define UPPER_LEVEL 5.0
#define LOWER_LEVEL 15.0

enum WaterStatus {LACKING = -1, OK, OVERFILLED, FILLING, PUMPING_OUT};

class WaterLevel: public Sensor
{
	public:
		WaterLevel(int echo, int trig, int id_sensor);
		bool make_reading();

		WaterStatus get_water_status();

	private:
		int _echo;
		int _trig;
};

#endif
