#ifndef Reading_h
#define Reading_h

#include <Arduino.h>

struct Reading
{
    int id_sensor;
    int id_measure;

    float value;
    char timestamp[16];

    void toJSON(char *target);
};

#endif
