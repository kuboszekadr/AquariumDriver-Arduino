#ifndef PhMeter_h
#define PhMeter_h

#include "Sensor.h"
#include <Arduino.h>

namespace Sensor
{
    class PhSensor : public Sensor
    {
    public:
        PhSensor(
            uint8_t pin,
            uint8_t id_sensor,
            Measures *id_measure,
            const char *name,

            float trigger_value_low,
            float trigger_value_high,

            uint32_t sampling_interval,
            uint8_t sampling_amount,

            Events::EventType trigger_low,
            Events::EventType trigger_high);
        bool makeReading();
        Events::EventType checkTriggers();

    private:
        uint8_t _pin;
    };
} // namespace Sensor
#endif