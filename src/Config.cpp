#include "Config.h"

#include "Config.h"

bool Config::loadFile(const char *file_name, JsonDocument &doc)
{
    File config_file = SD.open(file_name);
    if (!config_file)
    {
        Serial.println(F("Can not load file from SD"));
        return false;
    }

    DeserializationError error = deserializeJson(doc, config_file);
    if (error)
    {
        Serial.println(F("Failed to serialize config file"));
        return false;
    }

    config_file.close();
    return true; 
}

bool Config::saveToFile(const char *file_name, JsonDocument &doc)
{
    SD.remove(file_name);
    File file = SD.open(file_name, FILE_WRITE);

    if (!file)
    {
        Serial.print(F("Failed to create the file "));
        Serial.println(file_name);
        return false;
    }
    
    if (serializeJson(doc, file) == 0)
    {
        Serial.print(F("Failed to write to the file "));
        Serial.println(file_name);
        file.close();
        return false;
    }

    Serial.println(F("Config file updated."));
    file.close();    

    return true;
}