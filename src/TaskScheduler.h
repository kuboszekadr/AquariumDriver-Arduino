#ifndef TaskScheduler_h
#define TaskScheduler_h

#include "I2CSlave.h"
#include "Task.h"
#include "Log.h"

#include <Arduino.h>

#define TASK_SCHEDULER_MAX_TASKS 6
#define TASK_SCHEDULER_SCAN_INTERVAL 10000L  // scan tasks once a minute

namespace TaskScheduler
{
class Scheduler
{
public:
    static Scheduler &getInstance(); 
    void addTask(Task *task);
    void loop();

private:
    Scheduler(){};
    Task *_tasks[TASK_SCHEDULER_MAX_TASKS];
    uint8_t _tasks_amount = 0;
    uint32_t _last_scan = 0L;
};
} // namespace TaskScheduler

#endif