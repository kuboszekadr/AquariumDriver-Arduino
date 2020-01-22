#include "src/I2CSlave.h"

#include <Arduino.h>
#include <SoftwareSerial.h>

void setup() 
{
    Serial.begin(9600);
    Serial.println("Starting");
    i2c::setup();  // join I2C bus
    strcpy(i2c::response, "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed dignissim odio in orci lobortis tincidunt. Ut tempor magna a sollicitudin euismod. Sed hendrerit pellentesque nisl, quis iaculis mi blandit vel. Vivamus sed porta tellus.");
    Serial.println("Setup finished");
}

void loop()
{
    
    if (i2c::transmissionStep == i2c::FINISHED)
    {
        executeOrder();
        i2c::clearBuffer();
        i2c::transmissionStep = i2c::EMPTY;
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
    Serial.print("New order received - ");
    switch (i2c::order)
    {
    case i2c::UPDATE_RTC:
        Serial.println("RTC udate");
        break;
    case i2c::UPDATE_WIFI_STATUS:
        Serial.println("WiFI status change");
        break;
    case i2c::WATER_CHANGE:
        Serial.println("Water change");
        break;
    default:
        Serial.println("unknown command");
        Serial.println("Command message:");
        Serial.println(i2c::command);
        break;
    }

    // clear order
    i2c::order = i2c::UNKNOWN;
    return;
}