#include "WaterChange.h"

Programs::WaterChange::WaterChange(int pin_pomp, int pin_water)
{
    _pomp = new Relay(pin_pomp);
    _water = new Relay(pin_water);

    subscribe(Events::WATER_LOW);
    subscribe(Events::WATER_HIGH);
}

void Programs::WaterChange::changeWater()
{
    if (_is_active)
    {
        Serial.println(F("Program is already running!"));
        return;
    }

    _is_active = true;

    // check latest water status, if low only pour
    if (_state == Events::WATER_LOW)
    {
        pour();
    }
    else // otherwise, start with water pumping out
    {
        pumpOut();
    }
}

void Programs::WaterChange::pumpOut()
{
    _water->turnOff();
    _pomp->turnOn();
}

void Programs::WaterChange::pour()
{
    _water->turnOn();
    _pomp->turnOff();
}

void Programs::WaterChange::reactForEvent(Events::EventType event)
{
    // Check if program can be closed
    if (_state == Events::WATER_LOW && event == Events::WATER_HIGH)
    {
        _is_active = false;  // finish water change
        Serial.println(F("Closing water flow..."));
        _water->turnOff();
        _pomp->turnOff();
        _state = event;
    }
    //check if water has to be poured
    else if (event == Events::WATER_LOW)
    {
        Serial.println(F("Pouring water..."));
        _state = event;
        pour();
    }
    else if (_state == event)
    {
        // Do nothing...
    }
}
