#include "RTC.h"

RTC &RTC::getInstance()
{
    static RTC instance;
    return instance;
}

RTC &RTC::init(int rts, int clk, int dat)
{
    RTC &rtc = getInstance();

    // setup RTC pins
    rtc._rts = rts;
    rtc._clk = clk;
    rtc._dat = dat;

    rtc._RTCWire = new ThreeWire(rtc._dat, rtc._clk, rtc._rts);
    rtc._rtc = new RtcDS1302<ThreeWire>(*rtc._RTCWire);

    rtc._rtc->Begin();                   //init RTC clock
    rtc._rtc->SetIsWriteProtected(true); //make sure that clock is protected

    return rtc;
}

void RTC::setTimestamp(const char *timestamp)
{
    char year[5];
    char month[3];
    char day[3];

    char hour[3];
    char minute[3];
    char second[3];

    substring(year, timestamp, 0, 4);
    substring(month, timestamp, 4, 2);
    substring(day, timestamp, 6, 2);

    substring(hour, timestamp, 9, 2);
    substring(minute, timestamp, 11, 2);
    substring(second, timestamp, 13, 2);

    setTimestamp(atoi(year),
                 atoi(month),
                 atoi(day),
                 atoi(hour),
                 atoi(minute),
                 atoi(second) + 2);  // add extra seconds for processing time
}

void RTC::setTimestamp(uint32_t seconds)
{
    RTC &rtc = getInstance();

    rtc._rtc->SetIsWriteProtected(false);
    rtc._rtc->SetDateTime(seconds);
    rtc._rtc->SetIsWriteProtected(true);
}

void RTC::setTimestamp(int year, int month, int day, int hour, int minute, int second)
{
    RTC &rtc = getInstance();

    rtc._rtc->SetIsWriteProtected(false);
    rtc._rtc->SetDateTime(RtcDateTime(year, month, day, hour, minute, second));
    rtc._rtc->SetIsWriteProtected(true);
}

void RTC::getTimestamp(char *target)
{
    RTC &rtc = getInstance();

    RtcDateTime now = rtc._rtc->GetDateTime();
    sprintf(target,
            "%d%02d%02d %02d%02d%02d",
            now.Year(),
            now.Month(),
            now.Day(),
            now.Hour(),
            now.Minute(),
            now.Second());
}

uint32_t RTC::now()
{
    RTC &rtc = getInstance();
    RtcDateTime now = rtc._rtc->GetDateTime();
    return now.TotalSeconds();
}

DayOfWeek RTC::dayOfWeek()
{
    RTC &rtc = getInstance();
    RtcDateTime dt = rtc._rtc->GetDateTime();
    return dt.DayOfWeek();
}