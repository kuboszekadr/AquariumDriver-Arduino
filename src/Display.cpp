#include "Display.h"

Display Display::getInstance()
{
    static Display instance;
    return instance;
}

void Display::begin(int dc, int rst, int cs, Timestamp *timestamp)
{
    _timestamp = timestamp;

    _display = new Adafruit_SSD1306(OLED_SCREEN_WIDTH, OLED_SCREEN_HEIGHT, &SPI, dc, rst, cs);
    _display->begin(SSD1306_SWITCHCAPVCC);
    _display->clearDisplay();
    _display->setTextColor(WHITE);
    _display->setTextSize(1);

    _display->setCursor(0, 0);
    _display->display();
    show();
}

void Display::initRow(char *name, float *value)
{
    if (_rows_amount == OLED_SCREEN_MAX_ROWS)
    {
        return;
    }

    // create new row
    Row row;
    strcpy(row.name, name);
    row.value = value;

    // put it into rows array
    _rows[_rows_amount] = row;
    _rows_amount++;
}

void Display::show()
{
    if (millis() - _last_page_change >= OLED_SCREEN_PAGE_CHANGE_RATIO)
    {
        Serial.println("test");
        _page = (_page + 1) == OLED_PAGES_AMOUNT ? 0 : _page + 1;
        _last_page_change = millis();
        Serial.println(_page);
        Serial.println(OLED_PAGES_AMOUNT);
    }

    _display->clearDisplay();
    printTimestamp();
    uint8_t row = 0;

    // display page data
    for (uint8_t i = OLED_PAGES_AMOUNT * _page; i < OLED_PAGES_AMOUNT * _page + OLED_PAGE_ROWS; i++)
    {
        Serial.println(i);
        row++;
        _display->setCursor(0, row * 10 + 5);
        printRow(_rows[i]);
    }
    _display->display();
}

void Display::printRow(const Row &row)
{
    char msg[35];

    strcpy(msg, row.name);
    strcat(msg, " ");

    if (row.value != nullptr)
    {
        char value[10];
        dtostrf(*row.value, 2, 2, value);
        strcat(msg, value);
    }
    _display->print(msg);
}

void Display::printTimestamp()
{
    char ts[30];
    _timestamp->format(DateFormat::OLED, ts);
    _display->setCursor(0, 0);
    _display->print(ts);
}