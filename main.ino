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
        // execute order
    }
    else if (i2c::transmissionStep == i2c::TransmissionStep::EMPTY)
    {
        // get data from the sensors
    }
}

void scanSensors()
{

}

void executeOrder()
{
    
}