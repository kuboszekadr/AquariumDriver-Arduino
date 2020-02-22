#ifndef RTC_h
#define RTC_h

#include <Arduino.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>

class RTC
{
public:
    static RTC &getInstance();

    void set(int rts, int clk, int dat);

    static void setTimestamp(int year, int month, int day, int hour, int minute, int second);
    static void getTimestamp(char *timestamp);

private:
    RTC() {}
    RTC(const RTC &){};

    int _rts = -1;
    int _clk = -1;
    int _dat = -1;

    ThreeWire *_RTCWire;
    RtcDS1302<ThreeWire> *_rtc;



};

#endif