#ifndef Log_h
#define  Log_h

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Utils.h>

// #include <Sensor.h>

#include <RTC.h>
// #include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <SD.h>

enum LogLevel {APPLICATION = 0,  // print only data from the main 
                ERROR,  // show errors
                WARNING,   // warnings
                VERBOSE
            };  // all messages    

class Logger
{   
    public:    
        static Logger& getInstance();
        static void set_log_level(LogLevel log_level);

        static void set_rtc();
        // static void set_lcd(LiquidCrystal_I2C *lcd);
        static void set_sd(int pin);

        template <typename T>
        static void log(const T *msg);

        template <typename T>
        static void log(const T *msg, LogLevel log_level);

    private:
        Logger() {};
        Logger(const Logger&) {};

        // LiquidCrystal_I2C *_lcd;
        RTC* _rtc;

        template <typename T>
        void _print_serial(const T *msg, LogLevel log_level);

        template <typename T>
        void _print_sd(const T *msg, LogLevel log_level);

        // template <typename T>
        // void _print_lcd(const T *msg);

        int _sd_pin;
        char _log_file_name[20];
        // File _log_file;
        
        char _timestamp[17];
        LogLevel _log_level = ERROR;

        char _log_levels[4][10] = {"OUTPUT", "ERROR", "WARNING", "VERBOSE"};
};

template <typename T>
void Logger::log(const T *msg)
{
    log(msg, APPLICATION);
}

template <typename T>
void Logger::log(const T *msg, LogLevel log_level)
{
    Logger& logger = getInstance(); // get logger instance
    clear(logger._timestamp);

    logger._rtc->get_timestamp(logger._timestamp); // get current timestamp
    logger._print_serial(msg, log_level);
    logger._print_sd(msg, log_level);
}

template <typename T>
void Logger::_print_serial(const T *msg, LogLevel log_level)
{       
    Serial.print(_timestamp);
    Serial.print(F("\t"));
    
    Serial.print(_log_levels[log_level]);
    Serial.print(F(": "));
    
    Serial.println(msg);
}

template <typename T>
void Logger::_print_sd(const T *msg, LogLevel log_level)
{
    // char _log_file_name[20];
    memset(_log_file_name, 0, 20);  // erase file name from previous logging

    // create file name in format YYYYMMDD.txt
    substring(_log_file_name, _timestamp, 0, 8);  // take only YYYYMMDD info from the timestamp
    strcat(_log_file_name, ".txt");  // appent file extension

    File _log_file = SD.open(_log_file_name, FILE_WRITE);  // open file for writing if not exists it will create new

    if(_log_file)  // check if open is successfull
    {
        _log_file.print(_timestamp);
        _log_file.print(F("\t"));

        _log_file.print(_log_levels[log_level]);
        _log_file.print(F(": "));

        _log_file.println(msg);    
        _log_file.close();
    }
    else
    {
        Serial.print("Can not open the file: ");
        Serial.println(_log_file_name);
        if (SD.begin(_sd_pin))
        {
            Serial.println("SD reinitalization successfull");
        }
        else 
            Serial.println("Error");
    }
    
}

// template <typename T>
// void Logger::_print_lcd(const T *msg)
// {
//     static char logs[4][50];

//     Logger& logger = Logger::getInstance(); // get logger instance
//     logger._lcd->clear();

//     for (int i = 1; i < 4; i++)
//     {
//         logger._lcd->setCursor(0, i-1); 
//         logger._lcd->print(logs[i]);

//         memset(logs[i-1], 0, sizeof(logs[i-1]));   // clear data before offseting   
//         strcpy(logs[i-1], logs[i]);  // offset logs by one    
//     }
    
//     memset(logs[3], 0, sizeof(logs[3]));   // clear data before offseting     
//     _rtc.get_timestamp(logs[3]);
//     logger._lcd->print(logs[3]);
// }
#endif