#ifndef Logger_h
#define Logger_h

#include "RTC.h"
#include "Utils.h"

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>

#define LOG_BUFFER 150
#define DEBUG

// Definitions of all log labels in PROGMEM
const char log_level_application_str[] PROGMEM = "APPLICATION";
const char log_level_task_str[] PROGMEM = "TASK";

const char log_level_data_str[] PROGMEM = "DATA";
const char log_level_event_str[] PROGMEM = "EVENT";

const char log_level_warning_str[] PROGMEM = "WARNING";
const char log_level_verbose_str[] PROGMEM = "VERBOSE";

const char* const log_level_labels[] PROGMEM = {
    log_level_application_str,
    log_level_task_str,

    log_level_data_str,
    log_level_event_str,

    log_level_warning_str,
    log_level_verbose_str
};

enum LogLevel
{
    APPLICATION = 0, // print only data from the main
    TASK,            // for task events
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
    void _write_to_sd();

    LogLevel _log_level = VERBOSE;

    char _msg[LOG_BUFFER];

#ifdef DEBUG 
    void _print_serial();
#endif
};

#endif