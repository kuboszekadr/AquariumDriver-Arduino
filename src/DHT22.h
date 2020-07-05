#ifndef DHT22_h
#define DHT22_h

#include "Sensor.h"
#include <Arduino.h>

namespace Sensor
{
    class DHT22 : public Sensor
    {
    public:
        DHT22(uint8_t pin, uint8_t id_sensor, Measures *id_measures,
              const char *name,
              float trigger_value_low, float trigger_value_high,
              Events::EventType trigger_low, Events::EventType trigger_high);

        bool makeReading();

    private:
        uint8_t _pin;
    };
} // namespace Sensor
#endif