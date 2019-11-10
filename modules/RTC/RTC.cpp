#include "RTC.h"

RTC& RTC::get_instance()
{
    static RTC instance;
    return instance;
}

void RTC::init(int rts, int clk, int dat)
{
    RTC& rtc = get_instance();

    rtc._rts = rts;
    rtc._clk = clk;
    rtc._dat = dat;

    rtc._RTCWire = new ThreeWire (rtc._dat, rtc._clk, rtc._rts);
    rtc._rtc = new RtcDS1302<ThreeWire> (*rtc._RTCWire);

    rtc._rtc->Begin(); //init RTC clock
    rtc._rtc->SetIsWriteProtected(true); //make sure that clock is protected    
}

// void RTC::set_date(int year, int month, int day, int hour, int minute, int second)
// {
//     _rtc->SetIsWriteProtected(false);
//     _rtc->SetDateTime(RtcDateTime(year, month, day, hour, minute, second));
//     _rtc->SetIsWriteProtected(true);
// }

void RTC::get_timestamp(char *timestamp)
{
    RTC& rtc = get_instance();

    RtcDateTime now = rtc._rtc->GetDateTime();
    sprintf(timestamp, "%d%02d%02d %02d%02d%02d", now.Year(), now.Month(), now.Day(), now.Hour(), now.Minute(), now.Second());
}