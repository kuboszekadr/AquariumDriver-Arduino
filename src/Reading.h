#ifndef Reading_h
#define Reading_h

#include <Arduino.h>

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
