#ifndef PhMeter_h
#define PhMeter_h

#define PH_SAMPLING_INTERVAL 1000

#include "Sensor.h"
#include <Arduino.h>

class PhSensor : public Sensor
{
public:
    PhSensor(int pin, int id_sensor,
             const char *name,
             float trigger_value_low, float trigger_value_high,
             Events::EventType trigger_low, Events::EventType trigger_high);
    bool makeReading();
    Events::EventType checkTriggers();

private:
    int _pin;
};

#endif