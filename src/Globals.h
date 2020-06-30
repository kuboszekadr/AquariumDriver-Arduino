#ifndef Globals_h
#define Globals_h

#include "Pins.h"

#include "I2CSlave.h"
#include "Events.h"

#include "DHT22.h"
#include "Display.h"

#include "Lighting.h"
#include "Log.h"

#include "Programs.h"
#include "PhSensor.h"

#include "Reading.h"
#include "RTC.h"

#include "Sensor.h"

#include "TaskScheduler.h"
#include "Thermometer.h"
#include "Timestamp.h"

#include "WaterChange.h"
#include "WaterLevel.h"

#include "Utils.h"

#include "RTC.h"

/*------ GLOBAL VARIABLES ------*/
extern RTC &rtc;
extern Display display;

extern Programs::Program heater;
extern Programs::WaterChange water_change;

//extern Thermometer thermometer;
extern WaterLevel water_level_sensor;
extern PhSensor ph_sensor;

extern DHT22 dht_cover_left;
extern DHT22 dht_cover_center;
extern DHT22 dht_cover_right;

extern TaskScheduler::Scheduler &scheduler;
extern uint32_t timestamp;

#endif