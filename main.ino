/*---------------------*/
#include "src/I2CSlave.h"
#include "src/Events.h"

#include "src/Log.h"

#include "src/Programs.h"
#include "src/PhSensor.h"

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
#define SD_PIN 53

// DS18B20 - Thermometer
#define THERMOMETER_PIN 2
#define THERMOMETER_SENSOR_ID 1
#define THERMOMETER_MEASURE_ID 1
#define THERMOMETER_TEMP_LOW 24.8
#define THERMOMETER_TEMP_HIGH 25.2
uint8_t thermometer_address[8] = {0x28, 0x25, 0x34, 0xE5, 0x8, 0x0, 0x0, 0x35};

Thermometer thermometer(THERMOMETER_PIN, thermometer_address, THERMOMETER_SENSOR_ID, THERMOMETER_MEASURE_ID,
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
#define WATER_LEVEL_MEASURE_ID 2

WaterLevel water_level_sensor(WATER_LEVEL_SENSOR_ECHO_PIN, WATER_LEVEL_SENSOR_TRIG_PIN, WATER_LEVEL_SENSOR_ID, WATER_LEVEL_MEASURE_ID,
                              "WaterLevelSump",
                              (float)WATER_LEVEL_LOW, (float)WATER_LEVEL_HIGH,
                              Events::EventType::WATER_LOW, Events::EventType::WATER_HIGH);

// Water change
Programs::WaterChange water_change = Programs::WaterChange(WATER_LEVEL_POMP_PIN, WATER_LEVEL_WATER_PIN);

// Scheduler & Tasks
TaskScheduler::Scheduler &scheduler = TaskScheduler::Scheduler::getInstance();
TaskScheduler::Task water_change_task = TaskScheduler::Task("WaterChange", changeWater);

// Ph sensor
#define PH_SENSOR_PIN 12

#define PH_SENSOR_CO2_PIN 30
#define PH_SENSOR_PH_LOW 6.5
#define PH_SENSOR_PH_HIGH 6.7

#define PH_SENSOR_SENSOR_ID 3
#define PH_SENSOR_MEASURE_ID 3

PhSensor ph_sensor(PH_SENSOR_PIN, PH_SENSOR_SENSOR_ID, PH_SENSOR_MEASURE_ID,
                   "PhSensor",
                   (float)PH_SENSOR_PH_LOW, (float)PH_SENSOR_PH_HIGH,
                   Events::EventType::PH_LOW, Events::EventType::PH_HIGH);

void setup()
{
    Logger::setSD(SD_PIN);
    Logger::log(F("Starting"), LogLevel::VERBOSE);

    water_change_task.schedule(10, 30);
    scheduler.addTask(&water_change_task);

    i2c::begin(I2C_ADDRESS); // join I2C bus
    Logger::log(F("Setup finished"), LogLevel::VERBOSE);
}

void loop()
{
    // check if i2c transmission is finished
    if (i2c::transmission_step == i2c::FINISHED)
    {
        Logger::log(F("I2C transmission finished"), LogLevel::APPLICATION);
        Serial.println(strlen(i2c::data_buffer));

        // if tranmission yield with order execute.
        if (i2c::order > i2c::NONE)
        {
            Logger::log(F("Executing order: "), LogLevel::VERBOSE);
            executeOrder();
        }
        else if (i2c::order == i2c::UNKNOWN)
        {
            Logger::log(F("Unknown order command"), LogLevel::APPLICATION);
            Logger::log(i2c::command_buffer, LogLevel::APPLICATION);
        }

        i2c::clearBuffer();
        i2c::order = i2c::NONE;
        i2c::transmission_step = i2c::EMPTY;
    }
    // do not scan sensors when I2C transmission is in progress
    else if (i2c::transmission_step == i2c::EMPTY)
    {
        scanSensors(); // get data from the sensors

        // if event was added to the events queue, react
        Events::notifySubscribers();
        scheduler.loop(); // check for scheduled program runs
    }
}

void scanSensors()
{
    char reading_json[100]; // array to store reading of a sensor
    char msg[150];          // for logging messages
    char timestamp[16];    // reading timestamp
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
            Reading reading = sensor->getReading(); // average over available data
            strcpy(reading.timestamp, timestamp);   // add timestamp to the reading

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
            memset(timestamp, 0, 16);
        }
    }
}

// Schedule programs
void changeWater()
{
    Logger::log(F("Starting scheduled water change"), LogLevel::APPLICATION);
    water_change.changeWater();
}

void executeOrder()
{
    switch (i2c::order)
    {
    case i2c::Order::UPDATE_RTC: // update RTC
        RTC::setTimestamp(i2c::command_buffer+2);
        break;

    default:
        break;
    }
}