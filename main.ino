/*------ LOCAL LIBRARIES ------*/
#include "src/Pins.h"    // pins definisions
#include "src/Globals.h" // global variables initalisation
#include "src/Config.h"

/*------ GLOBAL LIBRARIES ------*/
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>

#include <SoftwareSerial.h>

#include <MemoryFree.h>

/*------ SENSORS IDs -----------*/
#define THERMOMETER_SENSOR_ID 1
#define WATER_LEVEL_SENSOR_ID 2
#define PH_SENSOR_SENSOR_ID 3

#define DHT_COVER_LEFT_SENSOR_ID 4
#define DHT_COVER_CENTER_SENSOR_ID 5
#define DHT_COVER_RIGHT_SENSOR_ID 6

/*------ PROGRAMS IDs -----------*/
#define PROGRAM_HEATER_ID 1
#define PROGRAM_WATER_CHANGE_ID 2
#define PROGRAM_CO2_ID 3

/*------ GLOBAL VARIABLES ------*/
Display display = Display::getInstance();
RTC &rtc = RTC::init(RTC_RTS_PIN, RTC_CLK_PIN, RTC_DAT_PIN);

// Heater
Events::EventType heater_programs[2] = {Events::EventType::TEMP_LOW, Events::EventType::TEMP_HIGH};
Programs::Program heater = Programs::Program(HEATER_RELAY_PIN, PROGRAM_HEATER_ID, heater_programs, 2);

Programs::WaterChange water_change = Programs::WaterChange(WATER_LEVEL_RELAY_POMP_PIN, WATER_LEVEL_RELAY_WATER_PIN, PROGRAM_WATER_CHANGE_ID);

// CO2
Events::EventType co2_programs[2] = {Events::EventType::PH_LOW, Events::EventType::PH_HIGH};
Programs::Program co2 = Programs::Program(PH_SENSOR_RELAY_CO2_PIN, PROGRAM_CO2_ID, co2_programs, 2);

TaskScheduler::Scheduler &scheduler = TaskScheduler::Scheduler::getInstance();

/*------ SENSOR INIT ------------*/
/*------ (by its' ID) ---------------*/

//DS18B20 - ThermometerR
#define THERMOMETER_TEMP_LOW 24.8
#define THERMOMETER_TEMP_HIGH 25.2
uint8_t thermometer_address[8] = {0x28, 0x25, 0x34, 0xE5, 0x8, 0x0, 0x0, 0x35};
Sensor::Measures t[1] = {Sensor::Measures::TEMP};
const char thermometer_sensor_name[] PROGMEM = "WaterTemp";

Sensor::Thermometer thermometer(
    THERMOMETER_PIN,
    thermometer_address,
    THERMOMETER_SENSOR_ID,
    t,
    thermometer_sensor_name,

    (float)THERMOMETER_TEMP_LOW,
    (float)THERMOMETER_TEMP_HIGH,

    1000L,
    30,

    Events::EventType::TEMP_LOW,
    Events::EventType::TEMP_HIGH);

// WATER LEVEL SENSOR - HC-SR04
#define WATER_LEVEL_LOW 20.0
#define WATER_LEVEL_HIGH 15.0

#define WATER_LEVEL_MEASURE_ID 2
Sensor::Measures water_level_measure[1] = {Sensor::Measures::WATER_LEVEL};
const char water_level_sensor_name[] PROGMEM = "WaterLevelSump";

Sensor::WaterLevel water_level_sensor(
    WATER_LEVEL_SENSOR_ECHO_PIN,
    WATER_LEVEL_SENSOR_TRIG_PIN,

    WATER_LEVEL_SENSOR_ID,
    water_level_measure,
    water_level_sensor_name,

    (float)WATER_LEVEL_LOW,
    (float)WATER_LEVEL_HIGH,

    1000L,
    30,

    Events::EventType::WATER_LOW,
    Events::EventType::WATER_HIGH);

// Ph sensor
#define PH_SENSOR_PH_LOW 6.5
#define PH_SENSOR_PH_HIGH 6.7

#define PH_SENSOR_MEASURE_ID 3
Sensor::Measures ph_measure[1] = {Sensor::Measures::PH};
const char ph_sensor_name[] PROGMEM = "WaterPh";

Sensor::PhSensor ph_sensor(
    PH_SENSOR_PIN,
    PH_SENSOR_SENSOR_ID,
    ph_measure,
    ph_sensor_name,

    (float)PH_SENSOR_PH_LOW,
    (float)PH_SENSOR_PH_HIGH,

    5000L,
    30,

    Events::EventType::PH_LOW,
    Events::EventType::PH_HIGH);

// DHT's
Sensor::Measures dht_measures[2] = {
    Sensor::Measures::TEMP,
    Sensor::Measures::HUMIDITY};

const char dht_cover_left_name[] PROGMEM = "CoverLeft";
const char dht_cover_center_name[] PROGMEM = "CoverCenter";
const char dht_cover_right_name[] PROGMEM = "CoverRight";

Sensor::DHT22 dht_cover_left(
    DHT_COVER_LEFT_PIN,
    DHT_COVER_LEFT_SENSOR_ID,
    dht_measures,
    dht_cover_left_name,

    0.0,
    0.0,

    2500L,
    30,

    Events::EventType::EMPTY,
    Events::EventType::EMPTY);

Sensor::DHT22 dht_cover_center(
    DHT_COVER_CENTER_PIN,
    DHT_COVER_CENTER_SENSOR_ID,
    dht_measures,
    dht_cover_center_name,

    0.0,
    0.0,

    2500L,
    30,

    Events::EventType::EMPTY,
    Events::EventType::EMPTY);

Sensor::DHT22 dht_cover_right(
    DHT_COVER_RIGHT_PIN,
    DHT_COVER_RIGHT_SENSOR_ID,
    dht_measures,
    dht_cover_right_name,

    0.0,
    0.0,

    2500L,
    30,

    Events::EventType::EMPTY,
    Events::EventType::EMPTY);

/*---------------------*/
void changeWater();
const char water_change_task_name[] PROGMEM = "WaterChange";
TaskScheduler::Task water_change_task = TaskScheduler::Task(water_change_task_name, changeWater);

uint32_t timestamp;
float i2c_buffer_size;

Lighting::Cover cover_left = Lighting::Cover(0, COVER_LEFT_PIN, 6);
Lighting::Cover cover_center = Lighting::Cover(1, COVER_CENTER_PIN, 8);
Lighting::Cover cover_right = Lighting::Cover(2, COVER_RIGHT_PIN, 6);

void setup()
{
    SD.begin(SD_PIN);
    Logger::log(F("Starting"), LogLevel::VERBOSE);
    
    // Load configs
    Sensor::loadConfig();
    Lighting::loadConfig();
    TaskScheduler::loadConfig();

    Lighting::start();

    // After loading config init OLED display
    display.begin(OLED_DC_PIN, OLED_RESET_PIN, OLED_CS_PIN, &timestamp);
    initDisplayRows();

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
            Logger::log(i2c::command_buffer, LogLevel::APPLICATION);
            executeOrder();
        }
        else if (i2c::order == i2c::UNKNOWN)
        {
            Logger::log(F("Unknown order command"), LogLevel::APPLICATION);
            Logger::log(i2c::command_buffer, LogLevel::APPLICATION);
        }
#ifdef DEBUG
        else
        {
            Serial.println(i2c::data_buffer);
        }
#endif
        i2c::clearBuffers();
        i2c::order = i2c::NONE;
        i2c::transmission_step = i2c::EMPTY;
    }
    // do not scan sensors when I2C transmission is in progress
    else if (i2c::transmission_step == i2c::EMPTY)
    {
        // get data from the sensors and check for events
        Sensor::loop();

        // if event has been added to the events queue, react
        Events::notifySubscribers();

        scheduler.loop(); // check for scheduled program runs
        Lighting::loop(); // run Lighting program changes

        timestamp = RTC::now();
        i2c_buffer_size = strlen(i2c::data_buffer);
        display.show(); // update display
    }

    if (i2c::isTimeouted())
    {
        Logger::log(F("I2C timeout..."), LogLevel::WARNING);
        i2c::transmission_step = i2c::EMPTY;
        i2c::clearBuffers();
    }
}

// Schedule programs
void changeWater()
{
    Logger::log(F("Starting water change"), LogLevel::APPLICATION);
    water_change.start();
}

void executeOrder()
{
    switch (i2c::order)
    {
    case i2c::Order::UPDATE_RTC:                    // update RTC
        RTC::setTimestamp(i2c::command_buffer + 2); // skip order ID = 1 and separator (;)
        break;
    case i2c::Order::WATER_CHANGE:
        changeWater();
        break;
    default:
        break;
    }
}

void initDisplayRows()
{
    // Page 1
    display.initRow(F("Temp"), thermometer.getReadings());
    display.initRow(F("Ph"), ph_sensor.getReadings());
    display.initRow(F("Water level"), water_level_sensor.getReadings());
    display.initRow(F("Buffer size"), &i2c_buffer_size);

    // Page 2
    display.initRow(F("Cover-left"), nullptr);
    display.initRow(F("Temp"), dht_cover_left.getReadings());
    display.initRow(F("Humidity"), (dht_cover_left.getReadings()) + 1);
    display.initRow(F(""), nullptr);

    // Page 3
    display.initRow(F("Cover-center"), nullptr);
    display.initRow(F("Temp"), dht_cover_center.getReadings());
    display.initRow(F("Humidity"), (dht_cover_center.getReadings()) + 1);
    display.initRow(F(""), nullptr);

    // Page 4
    display.initRow(F("Cover-right"), nullptr);
    display.initRow(F("Temp"), dht_cover_right.getReadings());
    display.initRow(F("Humidity"), (dht_cover_right.getReadings()) + 1);
    display.initRow(F(""), nullptr);
}
