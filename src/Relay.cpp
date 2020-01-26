#include "Relay.h"

Relay::Relay(int pin)
{
    _pin = pin;
    pinMode(_pin, OUTPUT);
    pinMode(_pin, LOW);
}

void Relay::turn_on()
{
    pinMode(_pin, HIGH);
}

void Relay::turn_off()
{
    pinMode(_pin, LOW);
}

int Relay::pin()
{
    return _pin;
}