#ifndef Programs_h
#define Programs_h

#include "Events.h"
#include "Relay.h"

#include <Arduino.h>

namespace Programs
{
class Program : public Events::EventSubscriber
{
public:
    Program() {};
    Program(int relay_pin, const Events::EventType *events_subscription, int events);
    
    void start();
    void end();
    void reactForEvent(Events::EventType event);
    
    bool isActive();

private:
    Relay *_relay;

protected:
    bool _is_active = false;
    Events::EventType _state;
};
} // namespace Programs
#endif
