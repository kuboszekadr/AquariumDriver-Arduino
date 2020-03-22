#include "Task.h"

TaskScheduler::Task::Task(const char *name, void (*fnc)())
{
    strcpy(_name, name);
    _fnc = fnc;
}

char *TaskScheduler::Task::getName()
{
    Serial.println(_name);
    return _name;
}

bool TaskScheduler::Task::isExecutable()
{
    // return false always when task is deactivated
    if (!_is_active)
    {
        return _is_active;
    }

    DayOfWeek day_of_week = RTC::dayOfWeek();
    Timestamp ts = RTC::now();

    // check if task was run during a day
    if (_last_run.getDate() == ts.getDate())
    {
        return false;
    }
    return _schedule[day_of_week] >= ts.getTime();
}

void TaskScheduler::Task::forceExecute()
{
    _fnc();
}

void TaskScheduler::Task::execute()
{
    forceExecute();
    Timestamp ts = RTC::now();

    // To simplify
    _last_run.year = ts.year;
    _last_run.month = ts.month;
    _last_run.day = ts.day;

    _last_run.hour = ts.hour;
    _last_run.minute = ts.minute;
    _last_run.second = ts.second;
}

void TaskScheduler::Task::activate()
{
    _is_active = true;
}

void TaskScheduler::Task::deactivate()
{
    _is_active = false;
}

void TaskScheduler::Task::schedule(uint8_t hour, uint8_t minute = 0)
{
    for (uint8_t day = 0; day <= DayOfWeek_Saturday; day++)
    {
        schedule(day, hour, minute);
    }
}

void TaskScheduler::Task::schedule(DayOfWeek day_of_week, uint8_t hour, uint8_t minute)
{
    _schedule[day_of_week] = (hour * 100L + minute) * 100L;
}