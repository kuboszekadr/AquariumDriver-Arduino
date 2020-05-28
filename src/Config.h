#ifndef Config_h
#define Config_h

#define CONFIG_SENSOR_FILE_SIZE 256

#include "Globals.h"

#include <ArduinoJson.h>
#include <SD.h>
#include <SPI.h>

namespace Config
{
    static const char sensor_config_file[] = "config/triggers.txt";
    void loadSensorConfig();
    void saveSensorConfig();
}

#endif