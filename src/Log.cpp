#include "Log.h"

Logger &Logger::getInstance()
{
    static Logger instance;
    return instance;
}

void Logger::log(const char *msg, LogLevel log_level)
{
    Logger &logger = getInstance();

    // check if log is to be displayed
    if (log_level < logger._log_level)
    {
        return;
    }

    logger._prepare(log_level);
    strcat(logger._msg, msg);
    logger._log();
}

void Logger::log(const __FlashStringHelper *msg, LogLevel log_level)
{
    Logger &logger = getInstance();

    // check if log is to be displayed
    if (log_level < logger._log_level)
    {
        return;
    }

    logger._prepare(log_level);
    strcat_P(logger._msg, (PGM_P)msg);
    logger._log();
}

void Logger::setSD(int pin)
{
    Logger &logger = getInstance();
    logger._sd_pin = pin;

    SD.begin(pin);
}

void Logger::_log()
{
    _print_serial();
    _write_to_sd();
}

void Logger::_print_serial()
{
    Serial.println(_msg);
}

void Logger::_write_to_sd()
{
    // TODO
}

void Logger::_prepare(LogLevel log_level)
{
    // clear previous log
    memset(_msg, 0, LOG_BUFFER);

    // get current timestamp
    RTC::getTimestamp(_msg);

    // prepare initial message for logging
    sprintf(_msg, "\t[%s]\t", LogLevelLabels[log_level]);
}