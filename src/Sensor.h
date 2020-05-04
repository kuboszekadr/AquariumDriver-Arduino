#ifndef Sensor_h
#define Sensor_h

#include "Events.h"
#include "Utils.h"
#include "Reading.h"

#include <Arduino.h>

// send new data in approx every 30s
#define SENSOR_SAMPLING_INTERVAL 1000L // sample every 1/2 second
#define SENSOR_SAMPLING_AMOUNT 5       // readings array size
#define SENSOR_AMOUNT 5                // maximum amount of sensors

class Sensor
{
public:
  static Sensor *sensors[SENSOR_AMOUNT]; // array of generated sensors
  static unsigned int sensors_amount;    // how many sensors are initalized
  static void collectData();             // collects data from all sensors

  Sensor(int id_sensor,
         int id_measure,
         const char *name,
         float trigger_value_low, float trigger_value_high,
         Events::EventType trigger_low, Events::EventType trigger_high);

  virtual bool makeReading() = 0;            // to be overwriten by the subclasses
  virtual Events::EventType checkTriggers(); // check if current level of sensor value is between low and high trigger

  Reading getReading(); // returns averaged value over sampling

  bool isAvailable(); // check if sensor gathered enough data
  bool isReady();     // check if sensor can gather data

  char *getName();

protected:
  float _trigger_value_low = -1.0; // average sensor value
  Events::EventType _trigger_low;  // event to be rised when sensor value is below low value

  float _trigger_value_high = -1.0; // +/- level when specify event should be published
  Events::EventType _trigger_high;  // event to be rised when sensor value is above value

  Events::EventType _last_trigger = Events::EventType::EMPTY;

  float _avg();                     // aggrage all readings within SENSOR_SAMPLING_AMOUNT
  float _last_reading_value = -1.0; // last readed value by the sensor

  float _readings[SENSOR_SAMPLING_AMOUNT]; // array to hold all readings done before publishing
  unsigned long _last_reading = 0;         // when last reading was done (as millis)

  int _readings_count = -1; // amount of readings done in the sesion

  int _id_sensor; // sensor id when exposing data by the Reading struct
  int _id_measure;

  char _name[20]; // sensor name / label

}; // namespace Sensor
#endif