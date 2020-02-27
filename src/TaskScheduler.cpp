#include "TaskScheduler.h"

TaskScheduler::Scheduler &TaskScheduler::Scheduler::getInstance()
{
    static Scheduler instance;
    return instance;
}

void TaskScheduler::Scheduler::addTask(Task *task)
{
    if (_tasks_amount >= MAX_TASKS)
    {
        return;
    }

    _tasks[_tasks_amount++] = task;
}

void TaskScheduler::Scheduler::loop()
{
    for (int i = 0; i < _tasks_amount; i++)
    {
        Task *task = _tasks[i];
        Serial.println(task->getName());
        task->forceExecute();
    }
}