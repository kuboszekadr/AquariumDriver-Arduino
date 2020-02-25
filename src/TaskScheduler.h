#ifndef TaskScheduler_h
#define TaskScheduler_h

#include "Task.h"

#define MAX_TASKS 6

namespace TaskScheduler
{
class Scheduler
{
public:
    void addTask(Task *task);
    void loop();
private:
    Task *_tasks[MAX_TASKS];
    unsigned int _tasks_amount = 0;
};
} // namespace TaskScheduler

#endif