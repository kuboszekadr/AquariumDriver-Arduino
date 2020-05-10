#ifndef Display_h
#define Display_h

#define OLED_SCREEN_WIDTH 128 // OLED display width, in pixels
#define OLED_SCREEN_HEIGHT 64 // OLED display height, in pixels

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <SPI.h>

class Display
{
public:
    static Display getInstance();

    void begin(int dc, int rst, int cs);

    void printTemp(float value);
    void printPh(float value);
    void printWaterLevel(float value);

    void printDateTime(char *datetime);
    void printOther(char *msg);

private:
    Display(){};

    void printValue(int row, float value, const char *value_name);
    void show();

    float _temp;
    float _ph;
    float _water_level;

    char datetime[20];
    char _msg[5][20];

    Adafruit_SSD1306 *_display;
};
#endif
