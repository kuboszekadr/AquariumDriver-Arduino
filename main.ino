#include "I2CSlave.h"

#include <Arduino.h>
#include <SoftwareSerial.h>

void setup() 
{
    i2c::setup();  // join I2C bus
}

void loop() 
{
    if (i2c::transmissionStep == i2c::TransmissionStep::FINISHED)
    {
        executeOrder();
    }
    else if (i2c::transmissionStep == i2c::TransmissionStep::EMPTY)
    {
        scanSensors();
    }
}

void scanSensors()
{

}

void executeOrder()
{
    switch (i2c::order)
    {
    case i2c::Order::UPDATE_RTC:
        // UPDATE RTC
        break;
    case i2c::Order::UPDATE_WIFI_STATUS:
        // UPDATE WIFI STATUS
        break;
    case i2c::Order::WATER_CHANGE:
        // START WATER CHANGE
        break;
    default:
        break;
    }

    // clear order
    i2c::order = i2c::Order::UNKNOWN;
    return;
}