#include "TaskScheduler.h"

TaskScheduler::Scheduler &TaskScheduler::Scheduler::getInstance()
{
    static Scheduler instance;
    return instance;
}

void TaskScheduler::Scheduler::loop()
{
    if (millis() - _last_scan < TASK_SCHEDULER_SCAN_INTERVAL)
    {
        return;
    }
    
    char msg[30];
    for (int i = 0; i < Task::tasks_amount; i++)
    {
        Task *task = Task::tasks[i];
        if (task->isExecutable())
        {        
            char task_name[TASK_NAME_LENGTH +1];
            task->getName(task_name);
            
            sprintf_P(msg, PSTR("Task %s is starting"), task_name);
            Logger::log(msg, LogLevel::APPLICATION);
            
            task->execute();
        }
    }
    _last_scan = millis();    
}