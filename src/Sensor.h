#ifndef Sensor_h
#define Sensor_h

#include "Events.h"
#include "Utils.h"
#include "Reading.h"

#include <Arduino.h>

// send new data in approx every 30s
#define SENSOR_SAMPLING_INTERVAL 1000L // sample every 1/2 second
#define SENSOR_SAMPLING_AMOUNT 2       // readings array size
#define SENSOR_AMOUNT 10               // maximum amount of sensors

enum Measures
{
  TEMP = 1,
  WATER_LEVEL,
  PH,
  HUMIDITY,
};

class Sensor
{
public:
  static Sensor *sensors[SENSOR_AMOUNT]; // array of generated sensors
  static uint8_t sensors_amount;         // how many sensors are initalized

  Sensor(uint8_t id_sensor,
         Measures *id_measure,
         uint8_t measures,
         const char *name,
         float trigger_value_low, float trigger_value_high,
         Events::EventType trigger_low, Events::EventType trigger_high);

  virtual bool makeReading() = 0;            // to be overwriten by the subclasses
  virtual Events::EventType checkTriggers(); // check if current level of sensor value is between low and high trigger

  Reading getReading(); // returns averaged value over sampling

  void setTriggerLow();
  void setTriggerHigh();

  bool isAvailable(); // check if sensor gathered enough data
  bool isReady();     // check if sensor can gather data

  char *getName();

protected:
  uint8_t _id_sensor; // sensor id when exposing data by the Reading struct

  float *_last_readings;       // to store
  float *_readings;            // array to hold all readings done before publishing
  uint8_t _readings_count = 0; // amount of readings done in the sesion

  uint8_t *_id_measures;
  uint8_t _measures_amount;

  float _trigger_value_low = -1.0; // average sensor value
  Events::EventType _trigger_low;  // event to be rised when sensor value is below low value

  float _trigger_value_high = -1.0; // +/- level when specify event should be published
  Events::EventType _trigger_high;  // event to be rised when sensor value is above value

  Events::EventType _last_trigger = Events::EventType::EMPTY;

  float _last_reading_value;
  unsigned long _last_reading = 0; // when last reading was done (as millis)

  char _name[20]; // sensor name / label
};                // namespace Sensor
#endif