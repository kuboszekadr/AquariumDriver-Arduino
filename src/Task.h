#ifndef TaskScheduler_Task_h
#define TaskScheduler_Task_h

#include "RTC.h"
#include "Timestamp.h"

#include <Arduino.h>
#include <RtcDateTime.h>

#define TASK_MAX_TASKS 1
#define TASK_NAME_LENGTH 20

namespace TaskScheduler
{
    class Task
    {
    public:
        static Task *tasks[TASK_MAX_TASKS];
        static uint8_t tasks_amount;

        Task(const char *name, void (*fnc)());

        char *getName();
        uint8_t getId();

        bool isExecutable(); // check if task can be executed

        void forceExecute(); // forces task execution regardless of it status
        void execute();

        void deactivate(); // deactive whole task
        void activate();   // active whole task

        void schedule(uint16_t hour);                        // set execution time the same for each day
        void schedule(DayOfWeek day_of_week, uint16_t hour); // time execution

    private:
        const char *_name; // name of the task
        void (*_fnc)();    // pointer to the function to be executed

        bool _is_active = true;  // is task activate
        uint32_t _schedule[7];   // array of days with scheduled execution dates
        uint32_t _last_run = 0L; // when task was run last time

        uint8_t _id;
    };
} // namespace TaskScheduler

#endif