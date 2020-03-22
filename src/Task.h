#ifndef TaskScheduler_Task_h
#define TaskScheduler_Task_h

#include "RTC.h"
#include "Timestamp.h"

#include <Arduino.h>
#include <RtcDateTime.h>

namespace TaskScheduler
{
class Task
{
public:
    Task(const char *name, void (*fnc)());

    char *getName();

    bool isExecutable(); // check if task can be executed

    void forceExecute(); // forces task execution regardless of it status
    void execute();

    void deactivate(); // deactive whole task
    void activate();   // active whole task

    void schedule(uint8_t hour, uint8_t minute = 0);                        // set execution time the same for each day
    void schedule(DayOfWeek day_of_week, uint8_t hour, uint8_t minute = 0); // time execution

private:
    char _name[16]; // name of the task
    void (*_fnc)(); // pointer to the function to be executed

    bool _is_active = true;     // is task activate
    unsigned long _schedule[7]; // array of days with scheduled execution dates
    Timestamp _last_run = {};         // when task was run last time
};
} // namespace TaskScheduler

#endif