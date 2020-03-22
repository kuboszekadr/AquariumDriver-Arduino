#include "Log.h"

Logger::Logger()
{
    Serial.begin(9600);
}

Logger &Logger::getInstance()
{
    static Logger instance;
    return instance;
}

void Logger::log(const char *msg, LogLevel log_level)
{
    Logger &logger = getInstance();

    // check if log is to be displayed
    if (log_level > logger._log_level)
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
    if (log_level > logger._log_level)
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
    // char timestamp[30];
    // RTC::getTimestamp(timestamp);

    // char log_file_name[12];
    // substring(log_file_name, timestamp, 0, 8);
    // strcat(log_file_name, ".txt");
/*
    File log_file = SD.open(log_file_name, FILE_WRITE);
    if (log_file)
    {
        log_file.println(_msg);
        log_file.close();
    }
    else
    {
        Serial.println(F("Cant access the SD card-reinitalizing"));
        SD.end();
        if (!SD.begin(_sd_pin))
        {
            Serial.println(F("Cant reinitalize SD card."));
        }
    }
*/
}

void Logger::_prepare(LogLevel log_level)
{
    // clear previous log
    memset(_msg, 0, LOG_BUFFER);

    // get current timestamp
    char timestamp[30];
    RTC::getTimestamp(timestamp);

    // prepare initial message for logging
    sprintf(_msg, "%s: [%s] ", timestamp, LogLevelLabels[log_level]);
}