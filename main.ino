#include "src/I2CSlave.h"
#include "src/Events.h"
#include "src/Programs.h"
#include "src/Reading.h"
#include "src/Sensor.h"
#include "src/Thermometer.h"
#include "src/WaterChange.h"
#include "src/WaterLevel.h"
#include "src/Utils.h"

#include <Arduino.h>
#include <SoftwareSerial.h>

// I2C
#define I2C_ADDRESS 8

// DS18B20 - Thermometer
#define THERMOMETER_PIN 2
#define THERMOMETER_SENSOR_ID 1
#define THERMOMETER_TEMP_LOW 24.8
#define THERMOMETER_TEMP_HIGH 25.2
uint8_t thermometer_address[8] = {0x28, 0x25, 0x34, 0xE5, 0x8, 0x0, 0x0, 0x35};
Thermometer thermometer(THERMOMETER_PIN, THERMOMETER_SENSOR_ID, thermometer_address,
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

Programs::WaterChange water_change = Programs::WaterChange(1, 2);
// Programs::Heater heater =

void setup()
{
    Serial.begin(9600);
    Serial.println("Starting");

    // i2c::begin(I2C_ADDRESS); // join I2C bus

    Serial.println("Setup finished");
}

void loop()
{
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
        if (Events::queueLength > 0)
        {
            Events::notifySubscribers();
        }
    }
}

void scanSensors()
{
    for (int i = 0; i < Sensor::sensors_amount; i++)
    {
        Sensor *sensor = Sensor::sensors[i];
        // check if sensor is reading for data collection
        if (sensor->isReady())
        {
            sensor->makeReading();
        }

        // check if sensor has collected enough data to share
        if (sensor->isAvailable())
        {
            Serial.print("Sensor ready: ");
            Serial.println(i);

            // request data from the sensor
            Reading r = sensor->getReading();
            Serial.println(r.value);

            (void)sensor->checkTriggers();
            addReadingToBuffer(&r); // add to I2C data buffer
        }
    }
}

void addReadingToBuffer(Reading *reading)
{
    char readingJSON[30];
    reading->toJSON(readingJSON); // get JSON string

    // if buffer is empty initalize JSON objects array
    unsigned int buffer_length = strlen(i2c::dataBuffer);

    if (buffer_length > 510)
    {
        Serial.println("I2C buffer full, can not send more data...");
        return; // avoid buffer overwride
    }
    else if (buffer_length + strlen(readingJSON) > 510)
    {
        Serial.println("Not enough space to fit data into the buffer");
        return;
    }
    else if (buffer_length > 0)
    {
        strcat(i2c::dataBuffer, ","); // add object separator
    }

    strcat(i2c::dataBuffer, readingJSON);
}