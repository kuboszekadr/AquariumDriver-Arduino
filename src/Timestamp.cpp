#include "Timestamp.h"

Timestamp::Timestamp(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
    this->year = year;
    this->month = month;
    this->day = day;

    this->hour = hour;
    this->minute = minute;
    this->second = second;
}

Timestamp::Timestamp(const RtcDateTime &dt)
{
    year = dt.Year();
    month = dt.Month();
    day = dt.Month();

    hour = dt.Hour();
    minute = dt.Minute();
    second = dt.Second();
}

void Timestamp::format(uint8_t format, char *taget)
{
    // TODO
}

void Timestamp::truncate(uint8_t level)
{
    if (level > 4)
    {
        return;
    }
    
    second = level <= 4 ? 0 : second;
    minute = level <= 3 ? 0 : minute;
    hour = level <= 2 ? 0 : hour;
    day = level <= 1 ? 0 : day;
    month = level = 0 ? 0 : month;
}

unsigned long Timestamp::getDate()
{
    unsigned long result = year * 10000L; // 10^4
    result += month * 100L;
    result += day * 1L;

    return result;
}

unsigned long Timestamp::getTime()
{
    unsigned long result = hour * 10000L; // 10^4
    result += month * 100L;
    result += second * 1L;

    return result; 
}

bool Timestamp::operator>(const Timestamp &t1) const
{
    if (getDate() > t1.getDate())
    {
        return true;
    }
    else if (getTime() > t1.getTime())
    {
        return true;   
    }
    
    return false;
}

bool Timestamp::operator<(const Timestamp &t1) const
{
    if (getDate() < t1.getDate())
    {
        return true;
    }
    else if (getTime() < t1.getTime())
    {
        return true;   
    }
    
    return false;
}

bool Timestamp::operator=(const Timestamp &t1) const
{
    if (year==t1.year &&
        month == t1.month && 
        day == t1.day && 
        hour == t1.hour && 
        minute == t1.minute && 
        second == t1.second)
    {
        return true;
    }
    else
    {
        return false;
    }
}