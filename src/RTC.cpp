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

void RTC::getTimestamp(char *timestamp)
{
    RTC &rtc = getInstance();

    RtcDateTime now = rtc._rtc->GetDateTime();
    sprintf(timestamp, "%d%02d%02d %02d%02d%02d", now.Year(), now.Month(), now.Day(), now.Hour(), now.Minute(), now.Second());
}