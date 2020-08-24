#ifndef TaskScheduler_h
#define TaskScheduler_h

#include "I2CSlave.h"
#include "Task.h"
#include "Log.h"

#include <Arduino.h>

#define TASK_SCHEDULER_SCAN_INTERVAL 500L // scan tasks once a minute
#define TASK_SCHEDULER_JSON_CLASS 1

namespace TaskScheduler
{
    class Scheduler
    {
    public:
        static Scheduler &getInstance();
        void loop();

    private:
        Scheduler(){};
        uint32_t _last_scan = 0L;
    };
} // namespace TaskScheduler

#endif