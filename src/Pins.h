#ifndef Pins_h
#define Pins_h

/*------ PINS DEFINISIONS ------*/
/*------ (by pin number)  ------*/  // TODO
#define I2C_ADDRESS 8 // I2C

// RTC
#define RTC_RTS_PIN 6
#define RTC_CLK_PIN 8
#define RTC_DAT_PIN 7

#define SD_PIN 53
// #define THERMOMETER_PIN 2
#define HEATER_RELAY_PIN 3

#define WATER_LEVEL_SENSOR_ECHO_PIN 3
#define WATER_LEVEL_SENSOR_TRIG_PIN 4

#define WATER_LEVEL_RELAY_POMP_PIN 1  //TODO
#define WATER_LEVEL_RELAY_WATER_PIN 2 //TODO

#define PH_SENSOR_PIN 12
#define PH_SENSOR_RELAY_CO2_PIN 30

#define DHT_COVER_LEFT_PIN 12 // TODO 
#define DHT_COVER_CENTER_PIN 13 // TODO
#define DHT_COVER_RIGHT_PIN 14 // TODO

#define OLED_DC_PIN 10
#define OLED_CS_PIN 11
#define OLED_RESET_PIN 9
/*------------------------------*/

#endif