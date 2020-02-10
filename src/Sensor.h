#ifndef Sensor_h
#define Sensor_h

#include "Events.h"
#include "Utils.h"
#include "Reading.h"

#include <Arduino.h>

// send new data in approx every 30s
#define SENSOR_SAMPLING_INTERVAL 500L // sample every 1/2 second
#define SENSOR_SAMPLING_AMOUNT 60     // readings array size
#define SENSOR_AMOUNT 5               // maximum amount of sensors

class Sensor
{
public:
  static Sensor *sensors[SENSOR_AMOUNT]; // array of generated sensors
  static unsigned int sensors_amount;    // how many sensors are initalized
  static void collectData();             // collects data from all sensors

  virtual bool makeReading() = 0;                // to be overwriten by the subclasses
  virtual Events::EventType checkTriggers() = 0; // check if current level of sensor value is between low and high trigger

  Sensor(int id_sensor, float trigger_low, float trigger_high);
  Reading getReading(); // returns averaged value over sampling

  bool isAvailable(); // check if sensor gathered enough data
  bool isReady();     // check if sensor can gather data

protected:
  float _trigger_low;  // average sensor value
  float _trigger_high; // +/- level when specify event should be published

  float _avg();              // aggrage all readings within SENSOR_SAMPLING_AMOUNT
  float _last_reading_value; // last readed value by the sensor

  float _readings[SENSOR_SAMPLING_AMOUNT]; // array to hold all readings done before publishing
  unsigned long _last_reading = 0;         // when last reading was done (as millis)

  int _readings_count = -1; // amount of readings done in the sesion
  int _id_sensor;           // sensor id when exposing data by the Reading struct

}; // namespace Sensor
#endif