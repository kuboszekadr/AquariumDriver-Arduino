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
}

char *TaskScheduler::Task::getName()
{
    char name[TASK_NAME_LENGTH + 1] = {};
    strncpy_P(name, (PGM_P)_name, TASK_NAME_LENGTH);
    Serial.println(name);
    return name;
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
    if (Timestamp::extract(DatePart::YYYYMMDD, _last_run) == now.extract(DatePart::YYYYMMDD))
    {
        return false;
    }
    return _schedule[day_of_week] <= now.extract(DatePart::HHMM);
}

void TaskScheduler::Task::execute()
{
    forceExecute();
    _last_run = RTC::now();
}

void TaskScheduler::Task::schedule(uint16_t hour)
{
    for (uint8_t day = 0; day <= DayOfWeek_Saturday; day++)
    {
        Serial.println(day);
        Serial.println(hour);
        schedule(day, hour);
    }
}

void TaskScheduler::Task::loadConfig()
{
    char file_name[12];
    strncpy(file_name, getName(), 8);
    Serial.println(file_name);

    char file_path[50];
    sprintf_P(file_path, config_path, file_name);
    Serial.println(file_path);

    StaticJsonDocument<TASK_JSON_SIZE> doc;
    if(!loadFile(file_path, doc))
    {
        return;
    }

    JsonArray schedules = doc.as<JsonArray>();
    for (JsonVariant day : schedules) 
    {
        Serial.println(day["d"].as<int>());
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
    sprintf(config_file, "config/tasks/%s.txt", file_name);
    
    saveToFile(config_file, doc);
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