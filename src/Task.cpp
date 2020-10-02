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

    tasks[tasks_amount++] = this;
    _lastRunEEPROMValueRead();
}

char *TaskScheduler::Task::getName()
{
    char name[TASK_NAME_LENGTH + 1] = {};
    strncpy_P(name, (PGM_P)_name, TASK_NAME_LENGTH);
    return name;
}

bool TaskScheduler::Task::isExecutable()
{
    _lastRunEEPROMValueRead();

    // return false always when task is deactivated
    if (!_is_active)
    {
        return _is_active;
    }

    DayOfWeek day_of_week = RTC::dayOfWeek();
    Timestamp now = Timestamp(RTC::now());

    // check if task was run during a day
    if (Timestamp::extract(DatePart::YYYYMMDD, _last_run.value) == now.extract(DatePart::YYYYMMDD))
    {
        return false;
    }
    return _schedule[day_of_week] <= now.extract(DatePart::HHMM);
}

void TaskScheduler::Task::execute()
{
    _last_run.value = RTC::now();
    _lastRunEEPROMValueSave(); // save last run date into EEPROM

    forceExecute();
}

void TaskScheduler::Task::loadConfig()
{
    char file_name[12] = {};
    strncpy(file_name, getName(), 8);

    char file_path[40] = {};
    sprintf_P(file_path, config_path, file_name);

    StaticJsonDocument<TASK_JSON_SIZE> doc;
    if(!loadFile(file_path, doc))
    {
        return;
    }

    JsonArray schedules = doc.as<JsonArray>();
    for (JsonVariant day : schedules) 
    {
        schedule(day["d"].as<int>(), day["h"].as<int>());
    }    
}

void TaskScheduler::Task::saveConfig()
{
    StaticJsonDocument<TASK_JSON_SIZE> doc;
    for (int i = 0; i < 7; i++)
    {
        JsonObject day_doc = doc.createNestedObject();
        day_doc["d"] = i;
        day_doc["h"] = _schedule[i];
    }

    char file_name[12];
    strncpy(file_name, getName(), 8);

    char config_file[40];
    sprintf_P(config_file, config_path, file_name);
    
    saveToFile(config_file, doc);
}

void TaskScheduler::Task::_lastRunEEPROMValueSave()
{
    // TODO: add task ID
    for (uint8_t i = 0; i < 4; i++)
    {
        EEPROM.write(i, _last_run.byte_array[i]);
    }
}

void TaskScheduler::Task::_lastRunEEPROMValueRead()
{
    // TODO: add task ID
    for (uint8_t i = 0; i < 4; i++)
    {
        _last_run.byte_array[i] = EEPROM.read(i);
    }
    Serial.println(_last_run.value);
}

void TaskScheduler::loadConfig()
{
    for (uint8_t i = 0; i < Task::tasks_amount; i++)
    {
        Task *task  = Task::tasks[i];
        task->loadConfig();
    }
}

void TaskScheduler::saveConfig()
{
    for (uint8_t i = 0; i < Task::tasks_amount; i++)
    {
        Task *task  = Task::tasks[i];
        task->saveConfig();
    }
}