#ifndef Pins_h
#define Pins_h

#define I2C_ADDRESS 8 // I2C

/*------ PINS DEFINISIONS ------*/
/*------ (by pin number)  ------*/

// COVERs
#define COVER_LEFT_PIN 2
#define COVER_CENTER_PIN 3
#define COVER_RIGHT_PIN 4

// RTC
#define RTC_DAT_PIN 23
#define RTC_CLK_PIN 22
#define RTC_RTS_PIN 24

// OLED SPI DISPLAY
#define OLED_DC_PIN 25
#define OLED_CS_PIN 26
#define OLED_RESET_PIN 27

// WATER LEVEL - HC 104
#define WATER_LEVEL_SENSOR_ECHO_PIN 28
#define WATER_LEVEL_SENSOR_TRIG_PIN 29

#define THERMOMETER_PIN 30

// DHTs
#define DHT_COVER_LEFT_PIN 31 
#define DHT_COVER_CENTER_PIN 32 // TODO
#define DHT_COVER_RIGHT_PIN 33 // TODO

#define SD_PIN 53

#define HEATER_RELAY_PIN 44

#define WATER_LEVEL_RELAY_POMP_PIN 48 
#define WATER_LEVEL_RELAY_WATER_PIN 49

#define PH_SENSOR_PIN A0
#define PH_SENSOR_RELAY_CO2_PIN 40

/*------------------------------*/

#endif