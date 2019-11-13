#ifndef PhMeter_h
#define  PhMeter_h

#define PH_SAMPLING_INTERVAL 1000

#include "Arduino.h"
#include "Sensor.h"
#include "Utils.h"
#include "RTC.h"

#define PH_UPPER_LEVEL 6.9
#define PH_LOWER_LEVEL 6.5

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