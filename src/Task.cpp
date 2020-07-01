#include "Task.h"

TaskScheduler::Task *TaskScheduler::Task::tasks[TASK_MAX_TASKS] = {};
uint8_t TaskScheduler::Task::tasks_amount = 0;

TaskScheduler::Task::Task(const char *name, void (*fnc)())
{
    _name = name;
    _fnc = fnc;

    if (tasks_amount == TASK_MAX_TASKS)
    {
        return;
    }

    _id = tasks_amount + 1;
    tasks[tasks_amount++] = this;
}

char *TaskScheduler::Task::getName()
{
    char name[TASK_NAME_LENGTH + 1] = {};
    strncpy_P(name, (PGM_P)_name, TASK_NAME_LENGTH);
    return name;
}

uint8_t TaskScheduler::Task::getId()
{
    return _id;
}

bool TaskScheduler::Task::isExecutable()
{
    // return false always when task is deactivated
    if (!_is_active)
    {
        return _is_active;
    }

    DayOfWeek day_of_week = RTC::dayOfWeek();
    Timestamp now = Timestamp(RTC::now());

    // check if task was run during a day
    if (Timestamp::extract(DatePart::HHMM, _last_run) == now.extract(DatePart::HHMM))
    {
        return false;
    }
    return _schedule[day_of_week] <= now.extract(DatePart::HHMM);
}

void TaskScheduler::Task::forceExecute()
{
    _fnc();
}

void TaskScheduler::Task::execute()
{
    forceExecute();
    _last_run = RTC::now();
}

void TaskScheduler::Task::activate()
{
    _is_active = true;
}

void TaskScheduler::Task::deactivate()
{
    _is_active = false;
}

void TaskScheduler::Task::schedule(uint16_t hour)
{
    for (uint8_t day = 0; day <= DayOfWeek_Saturday; day++)
    {
        schedule(day, hour);
    }
}

void TaskScheduler::Task::schedule(DayOfWeek day_of_week, uint16_t hour)
{
    _schedule[day_of_week] = hour;
}