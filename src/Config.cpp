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

void Config::loadLightingProgramsSetup()
{
    // Open the triggers.txt file which contains json with sensor low/high triggers
    File config_file = SD.open(lighting_config_file);
    if (!config_file) //check if file has been loaded successfully
    {
        Serial.println(F("Can not load config file"));
        return;
    }

    StaticJsonDocument<CONFIG_LIGHTING_FILE_SIZE> doc;
    DeserializationError error = deserializeJson(doc, config_file);

    if (error)
    {
        Serial.println(F("Failed to serialize config file"));
        return;
    }

    JsonArray json_programs = doc.as<JsonArray>();

    uint8_t i = 0;
    for (JsonVariant json_program : json_programs)
    {
        uint16_t program_start = json_program[0].as<int>();
        uint16_t program_end = json_program[1].as<int>();

        uint8_t program_start_cond[3] = {
            json_program[2][0].as<int>(),
            json_program[2][1].as<int>(),
            json_program[2][2].as<int>()};

        uint8_t program_end_cond[3] = {
            json_program[3][0].as<int>(),
            json_program[3][1].as<int>(),
            json_program[3][2].as<int>()};

        Lighting::Program *program = Lighting::programs[i++];
        program->setup(program_start,
                       program_end,
                       program_start_cond,
                       program_end_cond);
    }
}

void Config::saveLightingProgramsSetup()
{
    StaticJsonDocument<CONFIG_LIGHTING_FILE_SIZE> doc;
    JsonArray json_programs = doc.to<JsonArray>();

    // Loop through all programs and convert to JSON
    for (uint8_t i = 0; i < Lighting::programs_amount; i++)
    {
        Lighting::Program *program = Lighting::programs[i];
        if (!program)
        {
            continue;
        }

        // add program array to Json document
        JsonArray json_program = json_programs.createNestedArray();

        json_program.add(program->getStart()); // program start time
        json_program.add(program->getEnd());   // program end time

        // create nested array in progam Json to include pixel conditions
        JsonArray program_start_cond = json_program.createNestedArray();
        uint8_t *start_cond = program->getStartCondition();

        program_start_cond.add(start_cond[0]);
        program_start_cond.add(start_cond[1]);
        program_start_cond.add(start_cond[2]);

        // create nested array in progam Json to include pixel conditions
        JsonArray program_end_cond = json_program.createNestedArray();
        uint8_t *end_cond = program->getEndCondition();

        program_end_cond.add(end_cond[0]);
        program_end_cond.add(end_cond[1]);
        program_end_cond.add(end_cond[2]);
    }

    File file = SD.open(lighting_config_file, FILE_WRITE);
    if (!file)
    {
        Serial.println(F("Failed to open the file"));
        return;
    }

    if (serializeJson(doc, file) == 0)
    {
        Serial.println(F("Failed to write to file"));
    }
    Serial.println(F("Config updated"));
    file.close();
}