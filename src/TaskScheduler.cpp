#include "TaskScheduler.h"

TaskScheduler::Scheduler &TaskScheduler::Scheduler::getInstance()
{
    static Scheduler instance;
    return instance;
}

void TaskScheduler::Scheduler::addTask(Task *task)
{
    if (_tasks_amount >= TASK_SCHEDULER_MAX_TASKS)
    {
        Logger::log(F("Task list is full"), LogLevel::WARNING);
        return;
    }

    _tasks[_tasks_amount++] = task;
    Serial.println(_tasks_amount);
}

void TaskScheduler::Scheduler::loop()
{
    if (millis() - _last_scan > TASK_SCHEDULER_SCAN_INTERVAL)
    {
        return;
    }
    
    char msg[30];
    for (int i = 0; i < _tasks_amount; i++)
    {
        Task *task = _tasks[i];
        if (task->isExecutable())
        {        
            sprintf_P(msg, PSTR("Task %s is starting"), task->getName());
            Logger::log(msg, LogLevel::APPLICATION);
            task->execute();
        }
    }
    _last_scan = millis();    
}