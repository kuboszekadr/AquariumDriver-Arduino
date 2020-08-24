#include "Reading.h"

void Reading::toJSON(char *target)
{
    char _id_measure[3] = {};
    char _value[6] = {};

    // Loop through measures to get all readings
    char *readings = new char[21 * measures_amount](); // buffer for data readings

    for (uint8_t i = 0; i < measures_amount; i++)
    {
        char reading[20] = {};
        itoa(id_measures[i], _id_measure, 10);
        dtostrf(values[i], 2, 2, _value);

        // m - measure_id
        // v - reading_value
        sprintf_P(reading,
                PSTR("{\"m\":%s,\"v\":%s},"),
                _id_measure, _value);
                
        // copy into the buffer
        strcat(readings, reading);

        // clear buffers
        memset(_id_measure, 0, 3);
        memset(_value, 0, 6);
    }

    // remove last ',' in readings
    readings[strlen(readings) - 1] = '\0';

    char _id_sensor[3];
    itoa(id_sensor, _id_sensor, 10);

    // Prepare JSON to be exported later on:
    // Codes:
    // id - sensor_id
    // r - reading values
    // m - measure_id
    // v - reading_value
    // t - timestamp in YYYYMMDD HHmmSS format
    // Format: {"s":1,"r":["m":1:,"v":22.22],"t":20200513 063312}
    // using single chars save a lot in buffer
    sprintf_P(target,
            PSTR("{\"id\":%s,\"r\":[%s],\"t\":%s,\"c\":%d}"),
                _id_sensor, readings, timestamp, READING_JSON_CLASS);
    delete[] readings;
}
