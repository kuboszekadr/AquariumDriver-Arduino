#include "ReadingsQueue.h"

// returns first element from the queue
Reading ReadingsQueue::pop()
{
    if (_size == 0) 
        return; // return nothing if queue is empty
    
    Reading r = _queue[_front];
    _front = (_front + 1) % MAX_SIZE;
    _size--;

    return r;
}

// adds new reading in the end of the list
// TODO: change for & to make sure that non empty element is passed
bool ReadingsQueue::add(Reading *reading)
{
    if (_size == MAX_SIZE) 
        return false; // exit is queue full

    // add new element
    _size++;  // increase queue size
    _queue[_rear] = *reading;  // add element to the end
    _rear = (_rear + 1) % MAX_SIZE;  //get position for next element

    return true;
}

bool ReadingsQueue::is_empty()
{
    return (_size == 0);
}

bool ReadingsQueue::is_full()
{
    return (_size == MAX_SIZE);
}

// returns amount of elements in the queue 
int ReadingsQueue::size()
{
    return _size;
}

void ReadingsQueue::generate_post_request(char* post_data, const char* sep)
{
    if (_size == 0)
        return;

    // clear array before further processing
    memset(post_data, 0, sizeof(post_data));

    char id_sensors[50] = "id_sensor=";
    char readings[50] = "&value=";
    char timestamps[100] = "&timestamp=";

    char id_sensor[2];
    char reading[6];

    Reading r;
    while (!this->is_empty())
    {
        r = this->pop();    

        dtostrf(r.id_sensor, 1, 0, id_sensor);
        strcat(id_sensors, id_sensor);
        strcat(id_sensors, sep);

        dtostrf(r.value, 2, 2, reading);
        strcat(readings, reading);
        strcat(readings, sep);
        
        strcat(timestamps, r.timestamp);
        strcat(timestamps, sep);
    }

    //remove last separator
    id_sensors[strlen(id_sensors) - 1] = 0;
    readings[strlen(readings) - 1] = 0;
    timestamps[strlen(timestamps) - 1] = 0;    

    strcpy (post_data, id_sensors);
    strcat (post_data, readings);
    strcat (post_data, timestamps);
}
