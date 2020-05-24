#ifndef Timestamp_h
#define Timestamp_h

#include <Arduino.h>
#include <RtcDateTime.h>

enum DateFormat
{
    STANDARD, // YYYYMMDD
    JSON,     // YYYYMMDD hhmmss
    OLED   // YYYY-MM-DD hh:mm:ss
}; 

typedef struct Timestamp
{
    Timestamp(){};
    Timestamp(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
    Timestamp(const RtcDateTime &dt);

    void format(DateFormat format, char *target);
    void truncate(uint8_t level);

    unsigned long getDate();
    unsigned long getTime();

    bool operator>(const Timestamp &t1) const;
    bool operator<(const Timestamp &t1) const;
    bool operator=(const Timestamp &t1) const;

    uint16_t year = 0;
    uint8_t month = 0;
    uint8_t day = 0;

    uint8_t hour = 0;
    uint8_t minute = 0;
    uint8_t second = 0;
};

#endif