#ifndef TaskScheduler_h
#define TaskScheduler_h

#include "Task.h"

#include <Arduino.h>

#define MAX_TASKS 6

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
    Task *_tasks[MAX_TASKS];
    unsigned int _tasks_amount = 0;
};
} // namespace TaskScheduler

#endif