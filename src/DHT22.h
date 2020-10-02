#ifndef DHT22_h
#define DHT22_h

// #ifdef SENSOR_SAMPLING_INTERVAL
// #undef SENSOR_SAMPLING_INTERVAL 1000L // sample every 1 second
// #endif
// #define SENSOR_SAMPLING_INTERVAL 1000L

#include "DHT.h"
#include "Sensor.h"

#include <Arduino.h>

#define DHTTYPE DHT_22 // manually changed in DHT.h from DHT22

namespace Sensor
{
    class DHT22 : public Sensor, public DHT
    {
    public:
        DHT22(
            uint8_t pin,
            uint8_t id_sensor,
            Measures *id_measures,
            const char *name,

            float trigger_value_low,
            float trigger_value_high,

            uint32_t sampling_interval,
            uint8_t sampling_amount,

            Events::EventType trigger_low,
            Events::EventType trigger_high);

        bool makeReading();

    private:
        uint8_t _pin;
    };
} // namespace Sensor
#endif