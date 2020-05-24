#ifndef Display_h
#define Display_h

#define OLED_SCREEN_WIDTH 128 // OLED display width, in pixels
#define OLED_SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_SCREEN_MAX_ROWS 16
#define OLED_PAGE_ROWS 4
#define OLED_PAGES_AMOUNT OLED_SCREEN_MAX_ROWS / OLED_PAGE_ROWS 
#define OLED_SCREEN_PAGE_CHANGE_RATIO 3000L

#include "Timestamp.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <SPI.h>

class Display
{
public:
    static Display getInstance();

    void begin(int dc, int rst, int cs, Timestamp *timestamp);
    void initRow(char *name, float *value);
    void show();

private:
    struct Row
    {
        char name[20];
        float *value;
    };

    Display(){};

    void printRow(const Row &row);
    void printTimestamp();

    Row _rows[OLED_SCREEN_MAX_ROWS] = {};
    
    uint8_t _rows_amount = 0;
    uint8_t _page = 0;
    unsigned long _last_page_change = 0;

    Timestamp *_timestamp;

    Adafruit_SSD1306 *_display;
};
#endif
