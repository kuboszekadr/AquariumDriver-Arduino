#ifndef PhMeter_h
#define PhMeter_h

#define PH_SAMPLING_INTERVAL 1000

#include "Sensor.h"
#include <Arduino.h>

#define PH_UPPER_LEVEL 6.9
#define PH_LOWER_LEVEL 6.5

class PhSensor : public Sensor
{
public:
    PhSensor(int pin, int id_sensor,
             const char *name,
             float trigger_value_low, float trigger_value_high,
             Events::EventType trigger_low, Events::EventType trigger_high);
    bool make_reading();
    Events::EventType checkTriggers();

private:
    int _pin;
};

#endif