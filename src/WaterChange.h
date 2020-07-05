#ifndef WaterChange_h
#define WaterChange_h

#include "Programs.h"

namespace Programs
{
class WaterChange : public Program
{
public:
    WaterChange(uint8_t pin_pomp, uint8_t pin_water, uint8_t id);
    void start();
    void pumpOut();
    void pour();
    void reactForEvent(Events::EventType event);

private:
    Relay *_pomp;
    Relay *_water;
};
} // namespace Programs
#endif