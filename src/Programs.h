#ifndef Programs_h
#define Programs_h

#include "Events.h"
#include "I2CSlave.h"
#include "Log.h"

#include "Relay.h"
#include "RTC.h"

#include <Arduino.h>

#define PROGRAM_JSON_CLASS 1

namespace Programs
{
    class Program : public Events::EventSubscriber
    {
    public:
        Program(){};
        Program(uint8_t relay_pin, uint8_t id, const Events::EventType *events_subscription, uint8_t events);

        virtual void start();
        void end();
        void reactForEvent(Events::EventType event);

        bool isActive();

    protected:
        uint8_t _id;
        uint32_t _execution_id = 0;
        bool _is_active = false;
        Events::EventType _state;
        void addToI2CBuffer(const uint8_t active, const uint8_t step=1);

    private:
        Relay *_relay;
    };
} // namespace Programs
#endif
