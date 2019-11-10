#include "Relay.h"

Relay::Relay(int pin)
{
    _pin = pin;
    pinMode(_pin, OUTPUT);
    pinMode(_pin, LOW);
}

void Relay::turn_on()
{
    clear (_log_msg);
    sprintf(_log_msg, "Relay on pin: %d turning on.", _pin);
    Logger::log(_log_msg, LogLevel::VERBOSE);

    pinMode(_pin, HIGH);
}

void Relay::turn_off()
{
    clear (_log_msg);
    sprintf(_log_msg, "Relay on pin: %d turning off.", _pin);
    Logger::log(_log_msg, LogLevel::VERBOSE);

    pinMode(_pin, LOW);
}
