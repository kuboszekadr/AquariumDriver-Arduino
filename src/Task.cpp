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
    Timestamp now = Timestamp(RTC::now());

    // check if task was run during a day
    if (Timestamp::extract(DatePart::HHMM, _last_run) == now.extract(DatePart::HHMM))
    {
        return false;
    }
    return _schedule[day_of_week] >= now.extract(DatePart::HHMM);
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