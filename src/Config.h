#ifndef Config_h
#define Config_h

#define CONFIG_SENSOR_FILE_SIZE 256
#define CONFIG_LIGHTING_FILE_SIZE 500
#define ARDUINOJSON_ENABLE_COMMENTS 1

#include "Globals.h"

#include <ArduinoJson.h>

#include <SD.h>
#include <SPI.h>
#include <MemoryFree.h>

namespace Config
{
    static const char sensor_config_file[] = "triggers.txt";
    static const char lighting_config_file[] = "lighting.txt";

    void loadSensorConfig();
    void saveSensorConfig();

    void loadLightingProgramsSetup();
    void saveLightingProgramsSetup();
}

#endif