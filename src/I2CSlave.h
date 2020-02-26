#ifndef I2CSlave_h
#define I2CSlave_h

#define DATA_BUFFER_SIZE 1024
#define RESPONSE_BUFFER_SIZE 512

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
    EMPTY,   // to be set separatelly when slave is done with parsing the command
    ONGOING, // transmission is in progress
    FINISHED // all data received from the master
};

extern char dataBuffer[DATA_BUFFER_SIZE];           // for storing data from sensors
extern char commandBuffer[RESPONSE_BUFFER_SIZE]; // for storing commands from the master

extern TransmissionStep transmissionStep; // current transmission step
extern Order order;                       // task to be perfomed

void begin(int address); // initalize I2C
void receiveEvent(int count);
void requestEvent();

void clearBuffer();
bool addToBuffer(const char *data);

Order parseOrder();
} // namespace i2c

#endif