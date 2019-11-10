#include <Log.h>

static Logger& Logger::getInstance()
{
    static Logger instance;
    return instance;
}

void Logger::set_rtc()
{
    RTC& _rtc = RTC::get_instance();
}

void Logger::set_lcd(LiquidCrystal_I2C *lcd)
{
    Logger& logger = Logger::getInstance(); // get logger instance
    logger._lcd = lcd;
    logger._lcd->init();
    logger._lcd->backlight();
}

void Logger::set_sd(int pin)
{
    Logger& logger = Logger::getInstance(); // get logger instance
    logger._sd_pin = pin;
    SD.begin(pin);
}

void Logger::set_log_level(LogLevel log_level)
{
    LogLevel _log_level = log_level;
}

// void Logger::log(const char *msg)
// {
//     Logger& logger = getInstance(); // get logger instance
//     clear(logger._timestamp); // clear previous timestamp

//     logger._rtc.get_timestamp(logger._timestamp); // get current timestamp
//     logger._log_serial(msg); // print msg
//     logger._log_lcd(msg); // show msg on liquid crystal screen
//     logger._log_sd(msg); // save log to the file
// }

// void Logger::log(const __FlashStringHelper *msg)
// {
//     // Logger& logger = getInstance(); // get logger instance
//     // clear(logger._timestamp); // clear previous timestamp

//     // logger._rtc.get_timestamp(logger._timestamp); // get current timestamp
//     // logger._log_serial(msg); // print msg
//     // logger._log_lcd(msg); // show msg on liquid crystal screen
//     // logger._log_sd(msg); // save log to the file
// }

// void Logger::log(const char *msg, const char* timestamp)
// {
//     Logger& logger = Logger::getInstance(); // get logger instance
//     clear(logger._timestamp); // clear previous timestamp

//     strcpy(logger._timestamp, timestamp); // copy timestamp value
    
//     logger._log_serial(msg); // print msg
//     logger._log_sd(msg); // save log to the file
//     logger._log_lcd(msg); // show msg on liquid crystal screen
// }

// void Logger::_log_serial(const char *msg)
// {
//     Serial.print(_timestamp);
//     Serial.print(": ");
//     Serial.println(msg);
// }

// void Logger::_log_lcd(const char *msg)
// {
// }

// void Logger::_log_sd(const char *msg)
// {
//     Logger& logger = Logger::getInstance();
//     memset(logger._log_file_name, 0, 20);

//     //  create new file name YYYYMMDD.log
//     substring(logger._log_file_name, logger._timestamp, 0, 8);  // take only YYYYMMDD info from the timestamp
//     strcat(logger._log_file_name, ".txt");  

//     logger._log_file = SD.open(logger._log_file_name, FILE_WRITE);

//     if (logger._log_file)
//     {
//         logger._log_file.print(_timestamp);
//         logger._log_file.print(": ");
//         logger._log_file.println(msg);    
//         logger._log_file.close();
//     }
//     else
//     {
//         _log_serial("Can not open the file.");
//         SD.begin(logger._sd_pin);
//     }
// }

// void Logger::log(bool value)
// {
//     char val[2];
//     val[0] = value ? '1' : '0';

//     log(val);
// }
