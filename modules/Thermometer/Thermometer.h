#ifndef Thermometer_h
#define  Thermometer_h

#include "Arduino.h"
#include "OneWire.h"
#include "DS18B20.h"
#include "Sensor.h"

class Thermometer: public Sensor
{
        public:
                Thermometer(int pin, int id_sensor, char address[]);
                bool make_reading();
                bool ready();

        private:
                int _pin;
                char _address[8];// = {0x28, 0x25, 0x34, 0xE5, 0x8, 0x0, 0x0, 0x35};
                OneWire _onewire;
                DS18B20 *_sensor;
};

#endif

