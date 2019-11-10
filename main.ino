// Communication
#include <OneWire.h>
#include <ThreeWire.h>
#include <Wire.h> 
#include <ESP_8266.h>
#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <SD.h>

// Displays
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

// Sensors
#include <PhMeter.h>
#include <WaterLevel.h>
#include <Thermometer.h>

// Helpers
#include <Utils.h>
#include <Log.h>
#include <RTC.h>
#include <ReadingsQueue.h>

// Constants
#define SD_PIN 53

#define ESP_RX 13
#define ESP_TX 12

#define PH_PIN A0
#define PH_SENSOR_ID 3

#define THERMOMETER_SENSOR_ID 1
#define THERMOMETER_PIN 2

#define HC_ECHO 6
#define HC_TRIG 7
#define HC_SENSOR_ID 2

#define RTC_RTS 5
#define RTC_CLK 3 
#define RTC_DAT 4

// ESP configuration data
char HOST[14] = "192.168.0.179";
char SSID[20] = "Zdrajcy metalu";
char PWD[20] = "Dz3nt31m3n_m3ta1u";

ESP esp(ESP_RX, ESP_TX); // rx, tx

/* SENSORS */
PhMeter ph_meter(PH_PIN, PH_SENSOR_ID);
WaterLevel water_level_sump(HC_ECHO, HC_TRIG, HC_SENSOR_ID);

// DS18B20 - Thermometer
char address[8] = {0x28, 0x25, 0x34, 0xE5, 0x8, 0x0, 0x0, 0x35};
Thermometer thermometer(THERMOMETER_PIN, THERMOMETER_SENSOR_ID, address);

/* OTHER */
ReadingsQueue readings;  // to hold all readings to be sent
char msg[30];  // to create log messages

// Pixels
// Adafruit_NeoPixel lamp_rigth = Adafruit_NeoPixel(200, 40, NEO_GRB + NEO_KHZ800);
// Adafruit_NeoPixel lamp_middle = Adafruit_NeoPixel(200, 40, NEO_GRB + NEO_KHZ800);
// Adafruit_NeoPixel lamp_left = Adafruit_NeoPixel(200, 40, NEO_GRB + NEO_KHZ800);

void setup()
{
    delay(2000); // wait for boot
    Serial.begin(9600);  // available serial output

    RTC& rtc = RTC::get_instance(); 
    rtc.init(RTC_RTS, RTC_CLK, RTC_DAT);  // init RTC

    water_level_sump.attach_relay(8);  // attach relay
 
    LiquidCrystal_I2C *lcd = new LiquidCrystal_I2C(0x27, 20, 4);
    lcd->init();
    lcd->backlight();

    Logger::set_lcd(lcd);
    Logger::set_sd(SD_PIN);

    Logger::log(F("ESP - ready"));
    // Logger::log(esp.connected() ? "1" : "0");
    Serial.println(esp.connected() ? "1": "0");

    Logger::log(F("ESP - restarting"));
    (void) esp.restart();
    // Logger::log(esp.restart() ? "1" : "0");

    Logger::log(F("ESP - WiFi connection"));
    // Logger::log(esp.connect_wifi(SSID, PWD) ? "1" : "0");
    Serial.println(esp.connect_wifi(SSID, PWD) ? "1" : "0");


    Logger::log(F("Setup complete"));
}

void loop()
{
    check_water_level();
    read_temperature();
    send_data();

    if (esp.get_connection_status() == DISCONECTED)
    {
        Logger::log(F("Trying to reconnect"), LogLevel::APPLICATION);
        (void) esp.connect_wifi(SSID, PWD);
    }

    delay(500);
}

void check_water_level()
{
    // check if sensor is ready
    if(water_level_sump.ready())
    {
        Logger::log(F("Getting water level"), LogLevel::VERBOSE);
        (void)water_level_sump.make_reading();
    }
    
    // check if queue is full
    if(water_level_sump.available())
    {
        // get average water level
        struct Reading r = water_level_sump.get_reading();
        readings.add(&r);

        // show water level
        char water_level[6];
        dtostrf(r.value, 2, 2, water_level);

        clear(msg); // clear previous message
        strcpy(msg, "Water level: ");
        strcat(msg, water_level);
        Logger::log(msg, LogLevel::APPLICATION);  // log water level

        // check status
        clear(msg);  // clear previous message  

        WaterStatus water_status = water_level_sump.get_water_status(); 
        sprintf(msg, "Water status: %d", water_status);  
        Logger::log(msg, LogLevel::APPLICATION);  // log water status

        switch (water_status)
        {
        case LACKING:
            water_level_sump.relay->turn_on();  // open water flow
            break;
        case OVERFILLED:
            water_level_sump.relay->turn_off();  // close water flow
            break;
        default:
            water_level_sump.relay->turn_off();  // close water flow
            break;
        }
    }
}

void read_temperature()
{
    if (thermometer.ready())
    {
        Logger::log(F("Getting temp"), LogLevel::VERBOSE);
        thermometer.make_reading();  // make reading
    }

    if (thermometer.available())
    {
         // get average temperature
        struct Reading r = thermometer.get_reading();
        readings.add(&r);

        // show water level
        char temp[6];
        dtostrf(r.value, 2, 2, temp);

        clear(msg); // clear previous message
        strcpy(msg, "T: ");
        strcat(msg, temp);
        Logger::log(msg, LogLevel::APPLICATION);  // log water level       
        // Logger::log(r);
    }
}

void send_data()
{
    bool is_queue_empty = readings.is_empty();
    bool is_esp_connected = (esp.get_connection_status() != DISCONECTED);

    if (!is_queue_empty && is_esp_connected)
    {
        Logger::log(F("Sending data..."), LogLevel::APPLICATION);
        bool is_sent = esp.send_post_request(HOST, readings);

        if (is_sent)
            Logger::log(F("Data sent."), LogLevel::APPLICATION);
        else
            Logger::log(F("Failure."), LogLevel::ERROR);
    }
    // else if (is_queue_empty)
    //     Logger::log(F("Queue empty"));
    // else if (is_esp_connected)
    // {
    //     Logger::log(F("Disconected from AP"));
    // }
}
