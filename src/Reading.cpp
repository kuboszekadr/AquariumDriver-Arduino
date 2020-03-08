#include "Reading.h"

void Reading::toJSON(char *target)
{
    // {"id":1,"value":6.00}
    char _id_sensor[3];
    itoa(id_sensor, _id_sensor, 10);

    char _value[6];
    dtostrf(value, 2, 2, _value);

    sprintf(target, "{\"id\":%s,\"value\":%s,\"ts\":\"%s\"}", _id_sensor, _value, timestamp); // temporary constant value
}