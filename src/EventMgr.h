#ifdef EventMgr_h
#define EventMgr_h

#include "Events.h"

namespace Events
{
class EventMgr
{
public:
    Notify();

private:
    EventSubscriber EventSubscribers[10];
};
} // namespace Events

#endif