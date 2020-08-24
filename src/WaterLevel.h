/*
  WaterLevel.h - wraps HC-SR04 sensor to get water level
  Created by Adrian Kuboszek adrian.kuboszek@gmail.com 04.05.2019
  Last update: 
*/

#ifndef WaterLevel_h
#define WaterLevel_h

#include "Sensor.h"
#include <Arduino.h>

namespace Sensor
{
	class WaterLevel : public Sensor
	{
	public:
		WaterLevel(uint8_t echo, uint8_t trig, uint8_t id_sensor, Measures *id_measure,
				   const char *name,
				   float trigger_value_low, float trigger_value_high,
				   Events::EventType trigger_low, Events::EventType trigger_high);
		bool makeReading();
		Events::EventType checkTrigger();

	private:
		uint8_t _echo;
		uint8_t _trig;
	};
} // namespace Sensor
#endif
