#include "Sensor.h"

uint8_t Sensor::Sensor::sensors_amount = 0;
Sensor::Sensor *Sensor::Sensor::sensors[SENSOR_AMOUNT];

Sensor::Sensor::Sensor(uint8_t id_sensor,
                       Measures *id_measures,
                       uint8_t measures,
                       const char *name,

                       uint32_t sampling_interval,
                       uint8_t sampling_amount,

                       float trigger_value_low,
                       float trigger_value_high,

                       Events::EventType trigger_low,
                       Events::EventType trigger_high)
{
    if (sensors_amount == SENSOR_AMOUNT)
    {
        return;
    }

    sensors[sensors_amount] = this;                            // add sensor to the list of sensors
    sensors_amount += sensors_amount == SENSOR_AMOUNT ? 0 : 1; // increase amount of sensors

    _measures_amount = measures;
    _readings = new float[_measures_amount]();
    _last_readings = new float[_measures_amount]();
    _id_measures = new uint8_t[_measures_amount]();

    // convert measures enum into uint8_t
    for (uint8_t i = 0; i < _measures_amount; i++)
    {
        _id_measures[i] = static_cast<uint8_t>(id_measures[i]);
    }

    _name = name;
    _id_sensor = id_sensor;

    // setup trigger values and trigers
    _trigger_value_low = trigger_value_low;
    _trigger_low = trigger_low;

    _trigger_value_high = trigger_value_high;
    _trigger_high = trigger_high;

    _sampling_amount = sampling_amount;
    _sampling_interval = sampling_interval;
}

Reading Sensor::Sensor::getReading()
{
    // create reading data
    struct Reading reading;
    reading.id_sensor = _id_sensor;             // sensor id in database
    reading.id_measures = _id_measures;         // measures get from the sensor
    reading.measures_amount = _measures_amount; // technical to be able to loop through measures

    // calcualte average value over time
    for (uint8_t i = 0; i < _measures_amount; i++)
    {
        _readings[i] /= _readings_count; // calculate averages

        // make sure that reading values are between MIN and MAX theresholds
        _readings[i] = _readings[i] > SENSOR_THRESHOLD_MAX ? SENSOR_THRESHOLD_MAX : _readings[i];
        _readings[i] = _readings[i] < SENSOR_THRESHOLD_MIN ? SENSOR_THRESHOLD_MIN : _readings[i];

        _last_readings[i] = _readings[i]; // save values
        _readings[i] = 0.0;               // prepare for new data
    }

    reading.values = _last_readings; // copy it into reading struct
    _readings_count = 0;             // restart counter

    return reading;
}

void Sensor::Sensor::setTriggers(float trigger_value_low = -1.0, float trigger_value_high = -1.0)
{
    if (trigger_value_low > 0)
    {
        _trigger_value_low = trigger_value_low;
    }

    if (trigger_value_high > 0)
    {
        _trigger_value_high = trigger_value_high;
    }
}

void Sensor::Sensor::getName(char *buffer)
{
    strncpy_P(buffer, (PGM_P)_name, SENSOR_NAME_LENGHT);
}

Events::EventType Sensor::Sensor::checkTrigger()
{
    Events::EventType event = Events::EventType::EMPTY;
    if (_last_readings[0] < _trigger_value_low)
    {
        event = _trigger_low;
    }
    else if (_last_readings[0] > _trigger_value_high)
    {
        event = _trigger_high;
    }

    // push to the queue if event is not empty
    if (event != Events::EventType::EMPTY & event != _last_trigger)
    {
        Events::raise(event);
    }

    return event;
}

void Sensor::Sensor::saveConfig()
{
    StaticJsonDocument<SENSOR_JSON_SIZE> doc;
    doc["id"] = _id_sensor;
    doc["vh"] = _trigger_value_high;
    doc["vl"] = _trigger_value_low;

    char sensor_name[SENSOR_NAME_LENGHT + 1] = {};
    getName(sensor_name);

    char file_name[12] = {};
    memcpy(file_name, sensor_name, 8);

    char file_path[40];
    sprintf_P(file_path, config_path, file_name);

    saveToFile(file_path, doc);
}

void Sensor::Sensor::loadConfig()
{
    char file_name[9] = {};
    char sensor_name[SENSOR_NAME_LENGHT + 1] = {};

    getName(sensor_name);
    memcpy(file_name, sensor_name, 8);

    char file_path[40];
    sprintf_P(file_path, config_path, file_name);

    StaticJsonDocument<SENSOR_JSON_SIZE> doc;
    if (!loadFile(file_path, doc))
    {
        return;
    }

    _id_sensor = doc["id"].as<int>();
    _trigger_value_high = doc["vh"].as<float>();
    _trigger_value_low = doc["vl"].as<float>();
}

void Sensor::loadConfig()
{
    for (uint8_t i = 0; i < Sensor::sensors_amount; i++)
    {
        Sensor *sensor = Sensor::sensors[i];
        sensor->loadConfig();
    }
}

void Sensor::saveConfig()
{
    for (uint8_t i = 0; i < Sensor::sensors_amount; i++)
    {
        Sensor *sensor = Sensor::sensors[i];
        sensor->saveConfig();
    }
}

void Sensor::loop()
{
    char timestamp[20]; // reading timestamp
    RTC::getTimestamp(timestamp);

    Events::EventType event;
    char reading_json[50] = {}; // array to store reading of a sensor
    char msg[150] = {};         // for logging messages

    // loop through sensors
    for (int i = 0; i < Sensor::sensors_amount; i++)
    {
        Sensor *sensor = Sensor::sensors[i]; // take sensor
        char sensor_name[SENSOR_NAME_LENGHT + 1] = {};
        sensor->getName(sensor_name);

        // check if sensor is reading for data collection
        if (sensor->isReady())
        {
            if (sensor->makeReading())
            {
                sprintf_P(msg, PSTR("Sensor: %s reading successful"), sensor_name);
                Logger::log(msg, LogLevel::VERBOSE);
            }
            else
            {
                sprintf_P(msg, PSTR("Sensor: %s reading failed"), sensor_name);
                Logger::log(msg, LogLevel::WARNING);
            }
            memset(msg, 0, 150);
        }

        // check if sensor has collected enough data to share
        if (sensor->isAvailable())
        {
            sprintf_P(msg, PSTR("Sensor: %s ready"), sensor_name);
            Logger::log(msg, LogLevel::APPLICATION);

            Reading reading = sensor->getReading(); // average over available data
            reading.timestamp = timestamp;          // add timestamp to the reading

            // generate JSON and add to data buffer to be send to the database
            reading.toJSON(reading_json);
            i2c::addToBuffer(reading_json);
            Logger::log(reading_json, LogLevel::DATA);

            // check if some event has to be rised
            event = sensor->checkTrigger();

            // notify if needed
            if (event != Events::EventType::EMPTY)
            {
                char event_name[21] = {};
                Events::getEventLabel(event, event_name);
                Logger::log(event_name, LogLevel::EVENT);
            }

            memset(reading_json, 0, 50);
            memset(msg, 0, 150);
        }
    }
}