#include "Log.h"

Logger::Logger()
{
#ifdef DEBUG
    Serial.begin(9600);
#endif
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

void Logger::_log()
{
#ifdef DEBUG
    _print_serial();
#endif
    _write_to_sd();
}

void Logger::_print_serial()
{
    Serial.println(_msg);
}

void Logger::_write_to_sd()
{
    // Get current date from RTC
    char date[9];
    uint32_t now = RTC::now();
    Timestamp::format(DateFormat::STANDARD, date, now);

    // Create file name in YYYMMDD.txt format
    char log_file_name[12];
    substring(log_file_name, date, 0, 8);
    strcat(log_file_name, ".txt");

    // Load data into the file
    File log_file = SD.open(log_file_name, FILE_WRITE);
    if (log_file)
    {
        log_file.println(_msg);
        log_file.close();
    }
}

void Logger::_prepare(LogLevel log_level)
{
    // clear previous log
    memset(_msg, 0, LOG_BUFFER);

    // get current timestamp
    char timestamp[30];
    RTC::getTimestamp(timestamp);

    // prepare initial message for logging
    char label[12];
    strcpy_P(label, (char *)pgm_read_word(&(log_level_labels[log_level])));

    sprintf_P(_msg, 
        PSTR("%s: [%s] "), 
        timestamp, label);
}
