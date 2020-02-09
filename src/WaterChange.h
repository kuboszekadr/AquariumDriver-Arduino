#ifndef WaterChange_h
#define WaterChange_h

#include "Programs.h"

namespace Programs
{
class WaterChange : public Program
{
public:
    WaterChange(int pin_pomp, int pin_water);
    void changeWater();
    void pumpOut();
    void pour();
    void reactForEvent(Events::EventType event);

private:
    bool _is_active = false;
    Relay *_pomp;
    Relay *_water;
};
} // namespace Programs
#endif