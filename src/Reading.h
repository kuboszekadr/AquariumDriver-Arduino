#ifndef Reading_h
#define Reading_h

#include "RTC.h"

#include <Arduino.h>

#define READING_JSON_CLASS 0

struct Reading
{
    uint8_t id_sensor;
    uint8_t *id_measures;
    uint8_t measures_amount;

    float *values;
    char *timestamp;

    void toJSON(char *target);
};

#endif
