#include "Config.h"

#include "Config.h"

bool Config::loadFile(const char *file_name, JsonDocument &doc)
{
    if (!SD.exists(file_name))
    {
        Serial.println(file_name);
        Serial.println("does not exist");
    }

    File config_file = SD.open(file_name, FILE_READ);
    if (!config_file)
    {
        Serial.print(F("Could not open "));
        Serial.print(file_name);
        return false;
    }

    Serial.print(file_name);
    Serial.println(" loaded");

    DeserializationError error = deserializeJson(doc, config_file);
    if (error)
    {
        Serial.println(F("Failed to serialize JSON inside."));
        return false;
    }

    Serial.println(F("JSON serialized successfully"));

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