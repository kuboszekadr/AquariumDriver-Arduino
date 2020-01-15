#include "src/I2CSlave.h"

#include <Arduino.h>
#include <SoftwareSerial.h>

void setup() 
{
    i2c::setup();  // join I2C bus
}

void loop() 
{
    if (i2c::transmissionStep == i2c::FINISHED)
    {
        executeOrder();
    }
    else if (i2c::transmissionStep == i2c::EMPTY)
    {
        scanSensors();
    }
}

void scanSensors()
{
    // scan sensors
}

void executeOrder()
{
    switch (i2c::order)
    {
    case i2c::UPDATE_RTC:
        // UPDATE RTC
        break;
    case i2c::UPDATE_WIFI_STATUS:
        // UPDATE WIFI STATUS
        break;
    case i2c::WATER_CHANGE:
        // START WATER CHANGE
        break;
    default:
        break;
    }

    // clear order
    i2c::order = i2c::UNKNOWN;
    return;
}