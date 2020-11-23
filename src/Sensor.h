#ifndef Sensor_h
#define Sensor_h

#include "I2CSlave.h"
#include "Config.h"
#include "Events.h"
#include "Reading.h"
#include "RTC.h"
#include "Utils.h"

#include <Arduino.h>

#define SENSOR_AMOUNT 10 // maximum amount of sensors
#define SENSOR_NAME_LENGHT 20

#define SENSOR_THRESHOLD_MIN 0.0
#define SENSOR_THRESHOLD_MAX 99.99
#define SENSOR_JSON_SIZE 50

namespace Sensor
{
  enum Measures
  {
    TEMP = 1,
    WATER_LEVEL,
    PH,
    HUMIDITY
  };

  const char config_path[] PROGMEM = "config/sensors/%s.txt";

  class Sensor : public Config
  {
  public:
    static Sensor *sensors[SENSOR_AMOUNT] = {}; // array of generated sensors
    static uint8_t sensors_amount;              // how many sensors are initalized

    Sensor(uint8_t id_sensor,
           Measures *id_measure,
           uint8_t measures,
           const char *name,

           uint32_t sampling_interval,
           uint8_t sampling_amount,

           float trigger_value_low,
           float trigger_value_high,

           Events::EventType trigger_low,
           Events::EventType trigger_high);

    virtual bool makeReading() = 0;           // to be overwriten by the subclasses
    virtual Events::EventType checkTrigger(); // check if current level of sensor value is between low and high trigger

    Reading getReading(); // returns averaged value over sampling

    void setTriggers(float trigger_value_low = -1.0, float trigger_value_high = -1.0);
    void saveConfig();
    void loadConfig();

    float getTriggerValue(bool low) { return low ? _trigger_low : _trigger_high; };
    bool isAvailable() { return _readings_count >= _sampling_amount; };          // check if sensor gathered enough data
    bool isReady() { return (millis() - _last_reading) >= _sampling_interval; }; // check if sensor can gather data

    void getName(char *buffer);
    float *getReadings() { return _last_readings; };

  protected:
    uint8_t _id_sensor; // sensor id when exposing data by the Reading struct

    float *_last_readings;       // to store
    float *_readings;            // array to hold all readings done before publishing
    uint8_t _readings_count = 0; // amount of readings done in the sesion

    uint32_t _sampling_interval;
    uint8_t _sampling_amount;

    uint8_t *_id_measures;
    uint8_t _measures_amount;

    float _trigger_value_low = -1.0; // average sensor value
    Events::EventType _trigger_low;  // event to be rised when sensor value is below low value

    float _trigger_value_high = -1.0; // +/- level when specify event should be published
    Events::EventType _trigger_high;  // event to be rised when sensor value is above value

    Events::EventType _last_trigger = Events::EventType::EMPTY;

    uint32_t _last_reading = 0L; // when last reading was done (as millis)

    const char *_name; // sensor name / label
  };                   // namespace Sensor

  void loadConfig();
  void saveConfig();
  void loop();

} // namespace Sensor
#endif