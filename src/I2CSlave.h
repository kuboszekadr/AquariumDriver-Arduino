#ifndef I2CSlave_h
#define I2CSlave_h

#include "Utils.h"

#include <Arduino.h>
#include <Wire.h>

namespace i2c
{
    enum Order
    {
        UNKNOWN = -1,
        UPDATE_RTC,
        UPDATE_WIFI_STATUS,  
        WATER_CHANGE
    };

    enum TransmissionStep
    {
        EMPTY,  // to be set separatelly when slave is done with parsing the command
        ONGOING,  // transmission is in progress
        FINISHED  // all data received from the master
    };

    extern char dataBuffer[512];  // for storing data from sensors
    extern char commandBuffer[128];  // for storing commands from the master
    // extern char responseBuffer[128];  // for storing response message
    
    extern TransmissionStep transmissionStep;  // current transmission step
    extern Order order;  // task to be perfomed

    void begin(int address);  // initalize I2C
    void receiveEvent(int count);
    void requestEvent();

    void clearBuffer();

    Order parseOrder();
}

#endif