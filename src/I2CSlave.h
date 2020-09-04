#ifndef I2CSlave_h
#define I2CSlave_h

#define I2C_DATA_BUFFER_SIZE 512
#define I2C_COMMAND_BUFFER_SIZE 256
#define I2C_TIMEOUT 2000L

#include "Log.h"
#include "Utils.h"

#include <Arduino.h>
#include <Wire.h>

namespace i2c
{
    enum Order
    {
        UNKNOWN = -1,
        NONE,
        UPDATE_RTC,
        WATER_CHANGE
    };

    enum TransmissionStep
    {
        EMPTY,    // to be set separatelly when slave is done with parsing the command
        ONGOING,  // transmission is in progress
        FINISHED // all data received from the master
    };

    extern char data_buffer[I2C_DATA_BUFFER_SIZE];       // for storing data from sensors
    extern char command_buffer[I2C_COMMAND_BUFFER_SIZE]; // for storing commands from the master

    extern TransmissionStep transmission_step; // current transmission step
    extern Order order;                        // task to be perfomed

    void begin(int address); // initalize I2C
    void receiveEvent(int count);
    void requestEvent();

    void clearBuffers();
    void addToBuffer(const char *data);

    Order parseOrder();

    extern uint32_t last_request;
    bool isTimeouted();
} // namespace i2c

#endif