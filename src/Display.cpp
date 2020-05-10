#include "Display.h"

Display Display::getInstance()
{
    static Display instance;
    return instance;
}

void Display::begin(int dc, int rst, int cs)
{
    _display = new Adafruit_SSD1306(OLED_SCREEN_WIDTH, OLED_SCREEN_HEIGHT, &SPI, dc, rst, cs);
    _display->begin(SSD1306_SWITCHCAPVCC);
    _display->clearDisplay();
    _display->setTextColor(WHITE);
    _display->setTextSize(1);

    _display->setCursor(0,0);
    printTemp(0.0);
    printPh(0.0);
    printWaterLevel(0.0);

    _display->display();
}

void Display::printTemp(float value)
{
    printValue(0, value, "Temp");
}

void Display::printPh(float value)
{
    printValue(1, value, "Ph");
}

void Display::printWaterLevel(float value)
{
    printValue(2, value, "WaterLevel");
}

void Display::printOther(char *msg)
{
    strncpy(_msg[4], msg, 19);
    show();
}

void Display::printValue(int row, float value, const char *value_name)
{
    char msg[20];
    char _value[6];

    strcpy(msg, value_name);
    strcat(msg, ": ");

    dtostrf(value, 2, 2, _value);
    strcat(msg, _value);

    strcpy(_msg[row], msg);
    show();
}

void Display::show()
{
    _display->clearDisplay();

    for (int i = 0; i < 5; i++)
    {
        _display->setCursor(0, i*10+10);
        _display->print(_msg[i]);
    }

    _display->display();
}