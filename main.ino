/*------ LOCAL LIBRARIES ------*/
#include "src/Pins.h"  // pins definisions
#include "src/Globals.h" // global variables initalisation
#include "src/Config.h"

/*------ GLOBAL LIBRARIES ------*/
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <SoftwareSerial.h>

#include <MemoryFree.h>

/*------ GLOBAL VARIABLES ------*/
Display display = Display::getInstance();
RTC &rtc = RTC::init(RTC_RTS_PIN, RTC_CLK_PIN, RTC_DAT_PIN);

// Heater
Events::EventType heater_programs[2] = {Events::EventType::TEMP_LOW, Events::EventType::TEMP_HIGH};
Programs::Program heater = Programs::Program(HEATER_RELAY_PIN, heater_programs, 2);

Programs::WaterChange water_change = Programs::WaterChange(WATER_LEVEL_RELAY_POMP_PIN, WATER_LEVEL_RELAY_WATER_PIN);
TaskScheduler::Scheduler &scheduler = TaskScheduler::Scheduler::getInstance();

/*------ SENSORS IDs -----------*/
/*------ (by ID)   -----------*/
#define THERMOMETER_SENSOR_ID 1
#define WATER_LEVEL_SENSOR_ID 2
#define PH_SENSOR_SENSOR_ID 3

#define DHT_COVER_LEFT_SENSOR_ID 4
#define DHT_COVER_CENTER_SENSOR_ID 5
#define DHT_COVER_RIGHT_SENSOR_ID 6

/*------ SENSOR INIT ------------*/
/*------ (by its' ID) ---------------*/
// DS18B20 - Thermometer
// #define THERMOMETER_TEMP_LOW 24.8
// #define THERMOMETER_TEMP_HIGH 25.2
// uint8_t thermometer_address[8] = {0x28, 0x25, 0x34, 0xE5, 0x8, 0x0, 0x0, 0x35};
// Measures t[1] = {Measures::TEMP};

// Thermometer thermometer(THERMOMETER_PIN, thermometer_address, THERMOMETER_SENSOR_ID, t,
//                         "WaterTemp",
//                         (float)THERMOMETER_TEMP_LOW, (float)THERMOMETER_TEMP_HIGH,
//                         Events::EventType::TEMP_LOW, Events::EventType::TEMP_HIGH);

// WATER LEVEL SENSOR - HC-SR04
#define WATER_LEVEL_LOW 15.0
#define WATER_LEVEL_HIGH 10.0

#define WATER_LEVEL_MEASURE_ID 2
Measures water_level_measure[1] = {Measures::WATER_LEVEL};

WaterLevel water_level_sensor(WATER_LEVEL_SENSOR_ECHO_PIN, WATER_LEVEL_SENSOR_TRIG_PIN, WATER_LEVEL_SENSOR_ID, water_level_measure,
                              "WaterLevelSump",
                              (float)WATER_LEVEL_LOW, (float)WATER_LEVEL_HIGH,
                              Events::EventType::WATER_LOW, Events::EventType::WATER_HIGH);

// Ph sensor
#define PH_SENSOR_PH_LOW 6.5
#define PH_SENSOR_PH_HIGH 6.7

#define PH_SENSOR_MEASURE_ID 3
Measures ph_measure[1] = {Measures::PH};

PhSensor ph_sensor(PH_SENSOR_PIN, PH_SENSOR_SENSOR_ID, ph_measure,
                   "PhSensor",
                   (float)PH_SENSOR_PH_LOW, (float)PH_SENSOR_PH_HIGH,
                   Events::EventType::PH_LOW, Events::EventType::PH_HIGH);


// DHT's
Measures dht_measures[2] = {Measures::TEMP, Measures::HUMIDITY};

DHT22 dht_cover_left(DHT_COVER_LEFT_PIN, DHT_COVER_LEFT_SENSOR_ID, dht_measures,
                     "CoverLeft",
                     0.0, 0.0,
                     Events::EventType::EMPTY, Events::EventType::EMPTY);

DHT22 dht_cover_center(DHT_COVER_CENTER_PIN, DHT_COVER_CENTER_SENSOR_ID, dht_measures,
                       "CoverCenter",
                       0.0, 0.0,
                       Events::EventType::EMPTY, Events::EventType::EMPTY);

DHT22 dht_cover_right(DHT_COVER_RIGHT_PIN, DHT_COVER_RIGHT_SENSOR_ID, dht_measures,
                      "CoverRight",
                      0.0, 0.0,
                      Events::EventType::EMPTY, Events::EventType::EMPTY);

/*---------------------*/
void changeWater();
TaskScheduler::Task water_change_task = TaskScheduler::Task("WaterChange", changeWater);

uint32_t timestamp;
float i2c_buffer_size;

void setup()
{
    SD.begin(SD_PIN);
    Logger::log(F("Starting"), LogLevel::VERBOSE);

    Config::saveSensorConfig();
    Config::loadSensorConfig();

    display.begin(OLED_DC_PIN, OLED_RESET_PIN, OLED_CS_PIN, &timestamp);
    initDisplayRows();

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
        Serial.println(freeMemory());
    }
    // do not scan sensors when I2C transmission is in progress
    else if (i2c::transmission_step == i2c::EMPTY)
    {
        scanSensors(); // get data from the sensors

        // if event was added to the events queue, react
        Events::notifySubscribers();
        scheduler.loop(); // check for scheduled program runs
    }

    timestamp = RTC::now();
    display.show();
}

void scanSensors()
{
    char reading_json[150]; // array to store reading of a sensor
    char msg[150];          // for logging messages
    char _timestamp[20];     // reading timestamp
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
            RTC::getTimestamp(_timestamp);
            Reading reading = sensor->getReading(); // average over available data
            reading.timestamp = _timestamp;          // add timestamp to the reading

            // generate JSON and add to data buffer to be send to the database
            reading.toJSON(reading_json);
            i2c::addToBuffer(reading_json);

            Logger::log(reading_json, LogLevel::DATA);

            // check if some event has to be rised
            // event = Events::EventType::EMPTY; //sensor->checkTriggers();
            // if (event != Events::EventType::EMPTY)
            // {
            //     sprintf(msg, "%s", Events::EventTypeLabels[event]);
            //     Logger::log(msg, LogLevel::EVENT);
            // }

            i2c_buffer_size = strlen(i2c::data_buffer);

            memset(reading_json, 0, 150);
            memset(_timestamp, 0, 20);

            timestamp = RTC::now();
            display.show();
            Serial.println(freeMemory());
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
        RTC::setTimestamp(i2c::command_buffer + 2);
        break;

    default:
        break;
    }
}

void initDisplayRows()
{
    // Page 1
    display.initRow("Temp", (dht_cover_left.getReadings() + 1)); // to change to temp
    display.initRow("Ph", ph_sensor.getReadings());
    display.initRow("Water level", water_level_sensor.getReadings());
    display.initRow("Buffer size", &i2c_buffer_size);

    // Page 2
    display.initRow("Cover-left", nullptr);
    display.initRow("Temp", dht_cover_left.getReadings());
    display.initRow("Humidity", (dht_cover_left.getReadings()) + 1);
    display.initRow("", nullptr);

    // Page 3
    display.initRow("Cover-center", nullptr);
    display.initRow("Temp", dht_cover_center.getReadings());
    display.initRow("Humidity", (dht_cover_center.getReadings()) + 1);
    display.initRow("", nullptr);

    // Page 4
    display.initRow("Cover-right", nullptr);
    display.initRow("Temp", dht_cover_right.getReadings());
    display.initRow("Humidity", (dht_cover_right.getReadings()) + 1);
    display.initRow("", nullptr);
}