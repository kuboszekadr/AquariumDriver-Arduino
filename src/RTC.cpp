#include "RTC.h"

RTC &RTC::getInstance()
{
    static RTC instance;
    return instance;
}

void RTC::set(int rts, int clk, int dat)
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
    sprintf(target, "%d%02d%02d %02d%02d%02d", now.Year(), now.Month(), now.Day(), now.Hour(), now.Minute(), now.Second());
}

timestamp RTC::now(bool full = true)
{
    RTC &rtc = getInstance();

    timestamp result = 0;
    RtcDateTime now = rtc._rtc->GetDateTime();
    if (full)
    {
        result += now.Year() * 1000000000L; // 10^8
        result += now.Month() * 10000000L;  // 10^6
        result += now.Day() * 100000L;      // 10^4
    }

    result += now.Hour() * 100L; // 10^2
    result += now.Minute() * 1L; // 10^0

    return result;
}

DayOfWeek RTC::dayOfWeek()
{
    RTC &rtc = getInstance();
    RtcDateTime dt = rtc._rtc->GetDateTime();
    return dt.DayOfWeek();
}