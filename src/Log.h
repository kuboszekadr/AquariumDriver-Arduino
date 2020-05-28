#ifndef Logger_h
#define Logger_h

#include "RTC.h"
#include "Utils.h"

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>

#define LOG_BUFFER 150

enum LogLevel
{
    APPLICATION = 0, // print only data from the main
    DATA,            // data from sensors
    EVENT,           // log events
    WARNING,         // warnings
    VERBOSE          // all messages
};

class Logger
{
public:
    static Logger &getInstance();

    static void log(const char *msg, LogLevel log_level);
    static void log(const __FlashStringHelper *msg, LogLevel log_level);
    static void setLogLevel(LogLevel log_level);

private:
    Logger();

    void _prepare(LogLevel log_level);
    void _log();
    void _print_serial();
    void _write_to_sd();

    LogLevel _log_level = VERBOSE;

    char _msg[LOG_BUFFER];
    const char LogLevelLabels[5][12] = {"APPLICATION", "DATA", "EVENT", "WARNING", "VERBOSE"};
};

#endif