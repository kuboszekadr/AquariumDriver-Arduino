#ifndef Reading_h
#define Reading_h

#include <Arduino.h>

struct Reading
{
    int id_sensor;
    float value;
    char timestamp[17];

    void toJSON(char *target);
};

#endif