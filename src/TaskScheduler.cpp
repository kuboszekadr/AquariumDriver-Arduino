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
        Logger::log(F("Task list is full"), LogLevel::WARNING);
        return;
    }

    _tasks[_tasks_amount++] = task;
}

void TaskScheduler::Scheduler::loop()
{
    char msg[30];
    for (int i = 0; i < _tasks_amount; i++)
    {
        Task *task = _tasks[i];
        if (task->isExecutable())
        {        
            sprintf(msg, "Task %s is starting", task->getName());
            Logger::log(msg, LogLevel::APPLICATION);

            task->execute();
        }
    }
}