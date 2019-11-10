#ifndef ReadingsQueue_h
#define ReadingsQueue_h

#define MAX_SIZE 10

#include <Arduino.h>
#include <Sensor.h>
#include <Log.h>

class ReadingsQueue
{
    public:
        Reading pop();
        int size();

        bool add(Reading*);
        bool is_empty();
        bool is_full();

        void generate_post_request(char *post_data, const char *sep);

    private:
        Reading _queue[MAX_SIZE];
        int _front = 0;
        int _rear = 0;
        int _size = 0;
};

#endif