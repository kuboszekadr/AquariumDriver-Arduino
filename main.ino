#include "src/I2CSlave.h"
#include "src/Events.h"
#include "src/Programs.h"
#include "src/Reading.h"
#include "src/RTC.h"
#include "src/Sensor.h"
#include "src/Task.h"
#include "src/TaskScheduler.h"
#include "src/Thermometer.h"
#include "src/WaterChange.h"
#include "src/WaterLevel.h"
#include "src/Utils.h"

#include <Arduino.h>
#include <SoftwareSerial.h>

// I2C
#define I2C_ADDRESS 8

// RTC
#define RTC_RTS 6
#define RTC_CLK 8
#define RTC_DAT 7
RTC &rtc = RTC::init(RTC_RTS, RTC_CLK, RTC_DAT);

// DS18B20 - Thermometer
#define THERMOMETER_PIN 2
#define THERMOMETER_SENSOR_ID 1
#define THERMOMETER_TEMP_LOW 24.8
#define THERMOMETER_TEMP_HIGH 25.2
uint8_t thermometer_address[8] = {0x28, 0x25, 0x34, 0xE5, 0x8, 0x0, 0x0, 0x35};

Thermometer thermometer(THERMOMETER_PIN, thermometer_address, THERMOMETER_SENSOR_ID,
                        (float)THERMOMETER_TEMP_LOW, (float)THERMOMETER_TEMP_HIGH,
                        Events::EventType::TEMP_LOW, Events::EventType::TEMP_HIGH);

// WATER LEVEL SENSOR - HC-SR04
#define WATER_LEVEL_SENSOR_ECHO_PIN 3
#define WATER_LEVEL_SENSOR_TRIG_PIN 4
#define WATER_LEVEL_SENSOR_ID 2
#define WATER_LEVEL_LOW 15.0
#define WATER_LEVEL_HIGH 10.0

WaterLevel water_level_sensor(WATER_LEVEL_SENSOR_ECHO_PIN, WATER_LEVEL_SENSOR_TRIG_PIN, WATER_LEVEL_SENSOR_ID,
                              (float)WATER_LEVEL_LOW, (float)WATER_LEVEL_HIGH,
                              Events::EventType::WATER_LOW, Events::EventType::WATER_HIGH);

// Water change
Programs::WaterChange water_change = Programs::WaterChange(1, 2);

// Heater
Events::EventType heater_programs[2] = {Events::EventType::TEMP_LOW, Events::EventType::TEMP_HIGH};
Programs::Program heater = Programs::Program(10, heater_programs, 2);

TaskScheduler::Scheduler &scheduler = TaskScheduler::Scheduler::getInstance();

void changeWater()
{
    water_change.changeWater();
}

TaskScheduler::Task water_change_task = TaskScheduler::Task("WaterChange", changeWater);

char ts[30];

void setup()
{
    Serial.begin(9600);
    Serial.println("Starting");

    i2c::begin(I2C_ADDRESS); // join I2C bus

    Serial.println("Setup finished");
}

void loop()
{
    memset(ts, 0, 30);

    if (i2c::transmissionStep == i2c::FINISHED)
    {
        // executeOrder();
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
            // request data from the sensor
            RTC::getTimestamp(ts);
            Reading reading = sensor->getReading(); // average over available data
            strcpy(reading.timestamp, ts);          // add timestamp to the reading

            // generate JSON and add to data buffer to be send to the database
            reading.toJSON(reading_json);
            i2c::addToBuffer(reading_json);

            sensor->checkTriggers();
            memset(ts, 0, 30);
        }
    }
}
