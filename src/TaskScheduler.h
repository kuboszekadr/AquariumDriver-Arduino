#ifndef TaskScheduler_h
#define TaskScheduler_h

#include "Task.h"
#include "Log.h"

#include <Arduino.h>

#define TASK_SCHEDULER_MAX_TASKS 6
#define TASK_SCHEDULER_SCAN_INTERVAL 500L  // scan tasks once a minute

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
    unsigned int _tasks_amount = 0;
    unsigned long _last_scan = 0L;
};
} // namespace TaskScheduler

#endif