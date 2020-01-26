#ifndef Sensor_h
#define Sensor_h

#include "Utils.h"
#include "Relay.h"
#include "Reading.h"

#include <Arduino.h>

// send new data in approx every 30s
#define SENSOR_SAMPLING_INTERVAL 500L // sample every 1/2 second
#define SENSOR_SAMPLING_AMOUNT 5      // readings array size
#define SENSOR_AMOUNT 5               // maximum amount of sensors

class Sensor
{
public:

  static Sensor *sensors[SENSOR_AMOUNT];
  static unsigned int sensors_amount; // how many sensors are initalized

  static void collectData();      // collects data from all sensors
  virtual bool makeReading() = 0; // to be overwriten by the subclasses

  Sensor();
  Reading getReading();

  void attachRelay(int pin);
  Relay *relay;

  bool isAvailable(); // check if sensor gathered enough data
  bool isReady();     // check if sensor can gather data

protected:
  float _avg(); // aggrage all readings within SENSOR_SAMPLING_AMOUNT
  float _last_reading_value;

  float _readings[SENSOR_SAMPLING_AMOUNT];
  unsigned long _last_reading = 0; // when last reading was done (as millis)

  int _readings_count = -1;
  int _id_sensor = -1;

}; // namespace Sensors
#endif