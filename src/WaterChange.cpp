#include "WaterChange.h"

Programs::WaterChange::WaterChange(uint8_t pin_pomp, uint8_t pin_water, uint8_t id)
{
    _pomp = new Relay(pin_pomp);
    _water = new Relay(pin_water);

    _id = id;

    subscribe(Events::WATER_LOW);
    subscribe(Events::WATER_HIGH);
}

void Programs::WaterChange::start()
{
    if (_is_active)
    {
        Logger::log(F("Water change is already running."), LogLevel::VERBOSE);
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
    Logger::log(F("Pomping water our."), LogLevel::APPLICATION);
    _water->turnOff(); // close water flow
    _pomp->turnOn();   // turn the pomp on

    addToI2CBuffer(1, 1);
}

void Programs::WaterChange::pour()
{
    Logger::log(F("Pouring water."), LogLevel::APPLICATION);
    _pomp->turnOff(); // turn the pomp off
    _water->turnOn(); // open water flow

    addToI2CBuffer(1, 2);
}

void Programs::WaterChange::reactForEvent(Events::EventType event)
{
    // ignore the same state
    if (_state == event)
    {
        return;
    }

    // Check if program can be closed
    if (_state == Events::WATER_LOW && event == Events::WATER_HIGH)
    {
        Logger::log(F("Closing water flow..."), LogLevel::APPLICATION);

        _is_active = false; // finish water change
        
        // turn of the relays
        _water->turnOff(); 
        _pomp->turnOff();

        _state = event;
        addToI2CBuffer(0, 0); // inform about program ending
    }
    // check if water has to be poured
    else if (event == Events::WATER_LOW)
    {
        _state = event;
        pour();
    }
}
