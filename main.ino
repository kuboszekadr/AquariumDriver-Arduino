#include "src/Reading.h"
#include "src/Sensor.h"
#include "src/Thermometer.h"
#include "src/Utils.h"

#include <Arduino.h>
#include <SoftwareSerial.h>

// DS18B20 - Thermometer
uint8_t address[8] = {0x28, 0x25, 0x34, 0xE5, 0x8, 0x0, 0x0, 0x35};
Thermometer thermometer(2, 1, address);

void setup() 
{
    Serial.begin(9600);
    Serial.println("Starting");
    Serial.println("Setup finished");
}

void loop()
{
    Sensor::collectData();
}
