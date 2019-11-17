// Communication
#include <OneWire.h>
#include <ThreeWire.h>
#include <Wire.h> 
#include <ESP_8266.h>
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

ESP esp(ESP_RX, ESP_TX);  // rx, tx

/* SENSORS */
PhMeter ph_meter(PH_PIN, PH_SENSOR_ID);
WaterLevel water_level_sump(HC_ECHO, HC_TRIG, HC_SENSOR_ID);

// DS18B20 - Thermometer
uint8_t address[8] = {0x28, 0x25, 0x34, 0xE5, 0x8, 0x0, 0x0, 0x35};
Thermometer thermometer(THERMOMETER_PIN, THERMOMETER_SENSOR_ID, address);

/* OTHER */
ReadingsQueue readings;  // to hold all readings to be sent
char msg[30];  // to create log messages

// LCD display
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4);
char temp[6];
char ph[6];
char water_level[6];

void setup()
{
    delay(2000); // wait for boot
    Serial.begin(9600);  // available serial output

    RTC& rtc = RTC::get_instance(); 
    rtc.init(RTC_RTS, RTC_CLK, RTC_DAT);  // init RTC

    water_level_sump.attach_relay(8);  // attach relay
 
    lcd.init();
    lcd.backlight();
    lcd.print(F("Booting..."));

    Logger::set_sd(SD_PIN);

    Logger::log(F("ESP - ready"));
    Logger::log(esp.connected() ? "1" : "0");

    Logger::log(F("ESP - restarting"));
    Logger::log(esp.restart() ? "1" : "0");

    Logger::log(F("ESP - WiFi connection"));
    Logger::log(esp.connect_wifi(SSID, PWD) ? "1" : "0");

    Logger::log(F("Setup complete"));
    lcd.print(F("Setup complete..."));
    delay(500);
    lcd.clear();

}  // setup

void loop()
{
    check_water_level();
    read_temperature();
    read_ph();
    send_data();

    if (esp.get_connection_status() == DISCONECTED)
    {
        Logger::log(F("Trying to reconnect"), LogLevel::APPLICATION);
        (void) esp.connect_wifi(SSID, PWD);
    }

    print_lcd();
    delay(500);
} // loop

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
        dtostrf(r.value, 2, 2, water_level);

        clear(msg);  // clear previous message
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
    }  // if ready

    if (thermometer.available())
    {
         // get average temperature
        struct Reading r = thermometer.get_reading();
        readings.add(&r);

        // show water level
        dtostrf(r.value, 2, 2, temp);

        clear(msg); // clear previous message
        strcpy(msg, "T: ");
        strcat(msg, temp);
        Logger::log(msg, LogLevel::APPLICATION);  // log water level       
    }  // if available
}  // read temperature

void read_ph()
{
    if (ph_meter.ready())
    {
        Logger::log(F("Getting ph"), LogLevel::VERBOSE);
        ph_meter.make_reading();
    }

    if (ph_meter.available())
    {
        struct Reading r = ph_meter.get_reading();
        readings.add(&r);

        //show ph value
        dtostrf(r.value, 2, 2, ph);

        clear(msg);  // clear previous message
        strcpy(msg, "Ph: ");
        strcat(msg, ph);
        Logger::log(msg, LogLevel::APPLICATION);  // log water level       
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
}

void print_lcd()
{
 /*
    2019-11-11 21:12
    Temp: 26.6 'C
    Ph: 6.5
    WL: 15.65 cm
 */   
    char timestamp[20];
    RTC::get_timestamp(timestamp);

    timestamp[20] = '\0';  // truncate timestamp

    // Date time
    int row = 0;
    lcd.setCursor(0, row);
    lcd.print(timestamp);
    
    // Temperature
    row++;
    lcd.setCursor(0, row);
    lcd.print(F("Temp: "));
    lcd.setCursor(9, row);
    lcd.print(temp);

    // Ph
    row++;
    lcd.setCursor(0, row);
    lcd.print(F("Ph: "));
    lcd.setCursor(9, row);
    lcd.print(ph);

    // Water level
    row++;
    lcd.setCursor(0, row);
    lcd.print(F("WL: "));
    lcd.setCursor(9, row);
    lcd.print(water_level);
}