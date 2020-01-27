#ifndef Thermometer_h
#define Thermometer_h

#include "Sensor.h"

#include <Arduino.h>
#include <DS18B20.h>
#include <OneWire.h>

class Thermometer : public Sensor
{
public:
    Thermometer(int pin, int id_sensor, uint8_t address[]);
    bool makeReading();  // overwrite Sensor method
    bool isReady();

private:
    int _pin;
    uint8_t _address[8]; // = {0x28, 0x25, 0x34, 0xE5, 0x8, 0x0, 0x0, 0x35};
    OneWire _onewire;
    DS18B20 *_sensor;
};

#endif
