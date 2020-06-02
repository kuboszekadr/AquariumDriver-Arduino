#ifndef Timestamp_h
#define Timestamp_h

#include <Arduino.h>
#include <RtcDateTime.h>

enum DateFormat
{
    STANDARD, // YYYYMMDD
    JSON,     // YYYYMMDD hhmmss
    OLED,     // YYYY-MM-DD hh:mm:ss
    TIMEONLY  // hhmmss
};

enum DatePart
{
    YYYYMMDD,
    HHMMSS,
    HHMM,
    WK
};

class Timestamp : public RtcDateTime
{
public:
    Timestamp(uint32_t seconds);
    Timestamp(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);

    void format(DateFormat format, char *target);
    static void format(DateFormat format, char *target, uint32_t seconds);

    uint32_t extract(DatePart part);
    static uint32_t extract(DatePart part, uint32_t seconds);
};

#endif