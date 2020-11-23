#include "Timestamp.h"

Timestamp::Timestamp(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) : RtcDateTime(year, month, day, hour, minute, second)
{
}

Timestamp::Timestamp(uint32_t seconds) : RtcDateTime(seconds)
{
}

void Timestamp::format(DateFormat format, char *target)
{
    switch (format)
    {
    case STANDARD:
        sprintf(target,
                "%d%02d%02d",
                Year(), _month, _dayOfMonth);
        break;
    case OLED:
        sprintf(target,
                "%04d-%02d-%02d %02d:%02d:%02d",
                Year(),
                _month,
                _dayOfMonth,
                _hour,
                _minute,
                _second);
        break;
    default:
        break;
    }
}

void Timestamp::format(DateFormat format, char *target, uint32_t seconds)
{
    Timestamp ts = Timestamp(seconds);
    ts.format(format, target);
}

uint32_t Timestamp::extract(DatePart part)
{
    uint32_t result = 0L;
    switch (part)
    {
    case YYYYMMDD:
        result += Year() * 10000L;
        result += _month * 100L;
        result += _dayOfMonth * 1L;
        break;

    case HHMMSS:
        result += _hour * 10000L;
        result += _minute * 100L;
        result += _second * 1L;
        break;

    case HHMM:
        result += _hour * 100L;
        result += _minute * 1L;
        break;

    default:
        break;
    }
    return result;
}

uint32_t Timestamp::extract(DatePart part, uint32_t seconds)
{
    Timestamp ts = Timestamp(seconds);
    return ts.extract(part);
}
