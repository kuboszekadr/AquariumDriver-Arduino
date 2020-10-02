#ifndef Thermometer_h
#define Thermometer_h

#include "Sensor.h"

#include <Arduino.h>
#include <DS18B20.h>
#include <OneWire.h>

namespace Sensor
{
    class Thermometer : public Sensor
    {
    public:
        Thermometer(uint8_t pin,
                    uint8_t address[],
                    uint8_t id_sensor,
                    Measures *id_measure,
                    const char *name,

                    uint32_t sampling_interval,
                    uint8_t sampling_amount,

                    float trigger_value_low,
                    float trigger_value_high,

                    Events::EventType trigger_low,
                    Events::EventType trigger_high);

        bool makeReading();
        bool isReady(); // overwrite Sensor method

    private:
        uint8_t _pin;
        uint8_t _address[8] = {};
        OneWire _onewire;
        DS18B20 *_sensor;
    };
} // namespace Sensor
#endif
