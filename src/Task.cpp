#include "Task.h"

TaskScheduler::Task::Task(const char *name, void (*fnc)())
{
    strcpy(_name, name);
    _fnc = fnc;
}

bool TaskScheduler::Task::isExecutable()
{
    // return false always when task is deactivated
    if (!_is_active)
    {
        return _is_active;
    }

    DayOfWeek day_of_week = RTC::dayOfWeek();
    timestamp hour = RTC::now(false);

    bool result = false;
    result = _schedule[day_of_week] >= hour;                       // check if task can be executed
    result = result & (_last_run / 10000L != RTC::now() / 10000L); // check if task was run during a day
    return result;
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

void TaskScheduler::Task::schedule(int hour, int minute = 0)
{
    for (int day = 0; day <= DayOfWeek_Saturday; day++)
    {
        schedule(day, hour, minute);
    }
}

void TaskScheduler::Task::schedule(DayOfWeek day_of_week, int hour, int minute)
{
    _schedule[day_of_week] = hour * 100L + minute;
}