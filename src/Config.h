
#ifndef Config_h
#define Config_h

#include <ArduinoJson.h>

#include <SD.h>
#include <SPI.h>

class Config
{
    public:
        virtual void loadConfig();
        virtual void saveConfig();
    protected:
        bool loadFile(const char *file_name, JsonDocument &doc);
        bool saveToFile(const char *file_name, JsonDocument &doc);
};

#endif