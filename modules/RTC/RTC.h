#ifndef RTC_h
#define RTC_h

#include <Arduino.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>

class RTC
{
    public:
        static RTC& get_instance();
        
        void init(int rts, int clk, int dat);
        static void get_timestamp(char *timestamp);

    private:
        RTC() {}
        RTC(const RTC&) {};

        int _rts = -1;
        int _clk = -1;
        int _dat = -1;

        ThreeWire *_RTCWire;
        RtcDS1302<ThreeWire> *_rtc;
};

#endif