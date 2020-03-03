#ifndef Logger_h
#define Logger_h

#include "RTC.h"

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>

#define LOG_BUFFER 100

enum LogLevel
{
    APPLICATION = 0, // print only data from the main
    ERROR,           // show errors
    WARNING,         // warnings
    VERBOSE          // all messages
};

class Logger
{
    static Logger &getInstance();

    static void log(const char *msg, LogLevel log_level);
    static void log(const __FlashStringHelper *msg, LogLevel log_level);

    static void setLogLevel(LogLevel log_level);
    static void setSD(int pin);

private:
    Logger() {};

    int _sd_pin;

    void _prepare(LogLevel log_level);    
    void _log();
    void _print_serial();
    void _write_to_sd();

    LogLevel _log_level = VERBOSE;
    
    char _msg[LOG_BUFFER];
    char LogLevelLabels[4] = {'APPLICATION', 'ERROR', 'WARNING', 'VERBOSE'};
};

#endif