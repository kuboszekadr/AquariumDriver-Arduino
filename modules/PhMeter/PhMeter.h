#ifndef PhMeter_h
#define  PhMeter_h

#define SAMPLING_INTERVAL 1000

#include "Arduino.h"
#include "Sensor.h"
#include "Utils.h"
#include "RTC.h"

#define UPPER_LEVEL 6.7
#define LOWER_LEVEL 6.5

class PhMeter: public Sensor
{
    public:
        PhMeter(int pin, int id_sensor);
        bool make_reading();

    private:
        int _pin;
        int _id_sensor;
};

#endif  