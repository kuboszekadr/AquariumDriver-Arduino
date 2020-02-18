#ifndef Thermometer_h
#define Thermometer_h

#include "Sensor.h"

#include <Arduino.h>
#include <DS18B20.h>
#include <OneWire.h>

class Thermometer : public Sensor
{
public:
    Thermometer(int pin, uint8_t address[], int id_sensor,
                float trigger_value_low, float trigger_value_high,
                Events::EventType trigger_low, Events::EventType trigger_high);
    bool makeReading();
    bool isReady(); // overwrite Sensor method

private:
    int _pin;
    uint8_t _address[8] = {};
    OneWire _onewire;
    DS18B20 *_sensor;
};

#endif
