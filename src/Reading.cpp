#include "Reading.h"

void Reading::toJSON(char *target)
{
    // {"id":1,"value":6.00}
    char _id_sensor[3];
    dtostrf(id_sensor, 2, 0, _id_sensor);

    char _value[6];
    dtostrf(value, 2, 2, _value);

    sprintf(target, "{\"id\": %s,\"value\":%s, \"ts\":\"", _id_sensor, _value);  // temporary constant value
    strcat(target, timestamp);
    strcat(target, "}");
}