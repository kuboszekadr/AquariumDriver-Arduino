#ifndef TaskScheduler_Task_h
#define TaskScheduler_Task_h

#include "RTC.h"
#include "Timestamp.h"
#include "Config.h"

#include <Arduino.h>
#include <EEPROM.h>
#include <RtcDateTime.h>

#define TASK_MAX_TASKS 1
#define TASK_NAME_LENGTH 20
#define TASK_JSON_SIZE 256

namespace TaskScheduler
{
    const char config_path[] PROGMEM = "config/tasks/%s.txt";

    class Task : public Config
    {
    public:
        static Task *tasks[TASK_MAX_TASKS];
        static uint8_t tasks_amount;

        Task(const char *name, void (*fnc)());

        void getName(char *buffer);

        bool isExecutable(); // check if task can be executed

        void forceExecute() { _fnc(); }; // forces task execution regardless of it status
        void execute();

        void deactivate() { _is_active = false; }; // deactive whole task
        void activate() { _is_active = true; };    // active whole task

        void schedule(DayOfWeek day_of_week, uint16_t hour) { _schedule[day_of_week] = hour; }; // time execution

        void loadConfig();
        void saveConfig();

    private:
        // TODO: add task ID
        const char *_name; // name of the task
        void (*_fnc)();    // pointer to the function to be executed
        
        void _lastRunEEPROMValueSave();
        void _lastRunEEPROMValueRead();

        bool _is_active = true;  // is task activate
        uint32_t _schedule[7];   // array of days with scheduled execution dates
        
        // when task was run last time
        union last_run {
            uint32_t value = 0L;
            uint8_t byte_array[4];     
        };

        last_run _last_run;
    };

    void loadConfig();
    void saveConfig();
} // namespace TaskScheduler

#endif