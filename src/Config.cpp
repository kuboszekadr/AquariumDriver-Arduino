#include "Config.h"

void Config::loadSensorConfig()
{
    // Open the triggers.txt file which contains json with sensor low/high triggers
    File config_file = SD.open(sensor_config_file);
    if (!config_file) //check if file has been loaded successfully
    {
        Serial.println(F("Can not load config file"));
        return;
    }

    // deserialize config JSON
    StaticJsonDocument<256> config;
    DeserializationError error = deserializeJson(config, config_file);

    if (error)
    {
        Serial.println(F("Failed to serialize config file"));
        return;
    }

    // loop through sensor array and upload trigger values where needed
    JsonArray array = config.as<JsonArray>();
    for (JsonVariant v : array)
    {
        // for simplicity assume that sensor_id -1 is sensor place in the sensor array
        Sensor *sensor = Sensor::sensors[v["s"].as<int>() - 1];
        sensor->setTriggers(v["vl"].as<float>(), v["vh"].as<float>());
    }
    config_file.close();
}

void Config::saveSensorConfig()
{
    SD.remove(sensor_config_file);
    StaticJsonDocument<256> doc;
    JsonArray array = doc.to<JsonArray>();

    File file = SD.open(sensor_config_file, FILE_WRITE);
    if (!file)
    {
        Serial.println(F("Failed to create file"));
        return;
    }

    // using the assuption that sensors are ordered by ID
    // creating config data
    for (uint8_t i = 0; i < Sensor::sensors_amount; i++)
    {
        // retrieve sensor (use the assuption)
        Sensor *sensor = Sensor::sensors[i];
        JsonObject obj = array.createNestedObject(); // create new JSON object

        obj["s"] = i + 1; // IDs start from 1
        obj["vl"] = sensor->getTriggerValue(true);
        obj["vh"] = sensor->getTriggerValue(false);
    }

    // Serialize JSON to file
    if (serializeJson(doc, file) == 0)
    {
        Serial.println(F("Failed to write to file"));
    }

    Serial.println(F("Config updated"));
    // Close the file
    file.close();
}