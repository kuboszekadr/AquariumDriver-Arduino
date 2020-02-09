#ifndef Programs_h
#define Programs_h

#include "Events.h"
#include "Relay.h"

#include <Arduino.h>

namespace Programs
{
class Program : public Events::EventSubscriber
{
protected:
    Events::EventType _state;
};
} // namespace Programs
#endif
