#include "Relay.h"

Relay::Relay(int pin)
{
    _pin = pin;
    pinMode(_pin, OUTPUT);
    pinMode(_pin, LOW);
}

void Relay::turnOn()
{
    pinMode(_pin, HIGH);
}

void Relay::turnOff()
{
    pinMode(_pin, LOW);
}

int Relay::pin()
{
    return _pin;
}