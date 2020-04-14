/*---------------------*/
#include "src/I2CSlave.h"
#include "src/Events.h"

#include "src/Log.h"
#include "src/Programs.h"

#include "src/Reading.h"
#include "src/RTC.h"

#include "src/Sensor.h"

#include "src/Task.h"
#include "src/TaskScheduler.h"
#include "src/Thermometer.h"
#include "src/Timestamp.h"

#include "src/WaterChange.h"
#include "src/WaterLevel.h"

#include "src/Utils.h"

/*---------------------*/
#include <Arduino.h>
#include <SoftwareSerial.h>

/*---------------------*/

// Prototypes
void changeWater();

/*---------------------*/

// I2C
#define I2C_ADDRESS 8

/*---------------------*/
// RTC
#define RTC_RTS 6
#define RTC_CLK 8
#define RTC_DAT 7
RTC &rtc = RTC::init(RTC_RTS, RTC_CLK, RTC_DAT);

/*---------------------*/
//SD
#define SD_PIN 4

// DS18B20 - Thermometer
#define THERMOMETER_PIN 2
#define THERMOMETER_SENSOR_ID 1
#define THERMOMETER_TEMP_LOW 24.8
#define THERMOMETER_TEMP_HIGH 25.2
uint8_t thermometer_address[8] = {0x28, 0x25, 0x34, 0xE5, 0x8, 0x0, 0x0, 0x35};

Thermometer thermometer(THERMOMETER_PIN, thermometer_address, THERMOMETER_SENSOR_ID,
                        "WaterTemp",
                        (float)THERMOMETER_TEMP_LOW, (float)THERMOMETER_TEMP_HIGH,
                        Events::EventType::TEMP_LOW, Events::EventType::TEMP_HIGH);

// Heater
#define HEATER_PIN 3
Events::EventType heater_programs[2] = {Events::EventType::TEMP_LOW, Events::EventType::TEMP_HIGH};
Programs::Program heater = Programs::Program(HEATER_PIN, heater_programs, 2);

// WATER LEVEL SENSOR - HC-SR04
#define WATER_LEVEL_SENSOR_ECHO_PIN 3
#define WATER_LEVEL_SENSOR_TRIG_PIN 4

#define WATER_LEVEL_POMP_PIN 1
#define WATER_LEVEL_WATER_PIN 2

#define WATER_LEVEL_LOW 15.0
#define WATER_LEVEL_HIGH 10.0

#define WATER_LEVEL_SENSOR_ID 2

WaterLevel water_level_sensor(WATER_LEVEL_SENSOR_ECHO_PIN, WATER_LEVEL_SENSOR_TRIG_PIN, WATER_LEVEL_SENSOR_ID,
                              "WaterLevelSump",
                              (float)WATER_LEVEL_LOW, (float)WATER_LEVEL_HIGH,
                              Events::EventType::WATER_LOW, Events::EventType::WATER_HIGH);

// Water change
Programs::WaterChange water_change = Programs::WaterChange(WATER_LEVEL_POMP_PIN, WATER_LEVEL_WATER_PIN);

// Scheduler & Tasks
TaskScheduler::Scheduler &scheduler = TaskScheduler::Scheduler::getInstance();
TaskScheduler::Task water_change_task = TaskScheduler::Task("WaterChange", changeWater);

// Ph sensor
// TODO

void setup()
{
    Logger::log(F("Starting"), LogLevel::VERBOSE);

    water_change_task.schedule(10, 30);
    scheduler.addTask(&water_change_task);

    i2c::begin(I2C_ADDRESS); // join I2C bus
    Logger::setSD(SD_PIN);
    Logger::log(F("Setup finished"), LogLevel::VERBOSE);
}

void loop()
{
    if (i2c::transmissionStep == i2c::FINISHED)
    {
        Logger::log(F("I2C transmission finished"), LogLevel::APPLICATION);
        i2c::clearBuffer();
        i2c::transmissionStep = i2c::EMPTY;
    }
    // do not scan sensors when I2C transmission is in progress
    else if (i2c::transmissionStep == i2c::EMPTY)
    {
        scanSensors();
        Events::notifySubscribers();
        scheduler.loop();
    }
}

void scanSensors()
{
    char reading_json[50]; // array to store reading of a sensor
    char msg[50];          // for logging messages
    char timestamp[30];    // reading timestamp
    Events::EventType event;

    // loop through sensors
    for (int i = 0; i < Sensor::sensors_amount; i++)
    {
        Sensor *sensor = Sensor::sensors[i]; // take sensor
        // check if sensor is reading for data collection
        if (sensor->isReady())
        {
            sensor->makeReading();
        }

        // check if sensor has collected enough data to share
        if (sensor->isAvailable())
        {
            sprintf(msg, "Sensor: %s ready", sensor->getName());
            Logger::log(msg, LogLevel::APPLICATION);

            // request data from the sensor
            RTC::getTimestamp(timestamp);
            Reading reading = sensor->getReading();       // average over available data
            strcpy(reading.timestamp, timestamp); // add timestamp to the reading

            // generate JSON and add to data buffer to be send to the database
            reading.toJSON(reading_json);
            i2c::addToBuffer(reading_json);

            Logger::log(reading_json, LogLevel::DATA);

            // check if some event has to be rised 
            event = sensor->checkTriggers();
            if (event != Events::EventType::EMPTY)
            {
                sprintf(msg, "%s", Events::EventTypeLabels[event]);
                Logger::log(msg, LogLevel::EVENT);
            }
            memset(timestamp, 0, 30);
        }
    }
}

// Schedule programs
void changeWater()
{
    Logger::log(F("Starting scheduled water change"), LogLevel::APPLICATION);
    water_change.changeWater();
}
