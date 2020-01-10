#include "Utils.h"

float avg(const float *arr, const int arr_size)
{
    float sum = 0.0;

    for (int i = 0; i < arr_size; i++)
        sum += arr[i];  

    return sum / (arr_size*1.0);
}

// void clear(char *arr)
// {
//     int n = strlen(arr);
//     memset(arr, 0, n);
// }

// void read_serial(char *arr, int arr_size, SoftwareSerial *serial)
// {   
//     clear(arr); // erase array content

//     int i = 0;
//     while((serial->available() > 0) & i < (arr_size-1))
//         arr[i++] = serial->read(); // read incoming data   
//     arr[i] = 0; // add null termination string

//     // if(serial->available() & i >= (arr_size-1))
//     // {
//     //     // Logger::log(F("Serial response longer then array size."), LogLevel::WARNING);
//     //     // Logger::log(F("Leftover below:"), LogLevel::WARNING);
        
//     //     // TODO: add array to hold the response
//     //     // while((serial->available()))
//     //     //     Serial.print(serial->read());
//     //     // Serial.println();
//     // }       
// }

// void print_serial(const char *arr, SoftwareSerial *serial)
// {
//     const char *p;
//     p = arr;

//     while (*p)
//     {
//         serial->print(*p);
//         p++;
//     }   
//     serial->println();
//     serial->flush(); // wait for the data to be transfered
// }

// void clear_buffer(SoftwareSerial *serial)
// {
//     serial->flush();
//     while(serial->available())
//         serial->read();
// }

void substring(char *target, const char *str, int start, int length)
{
    int c = 0;
    while (c < length)
        target[c++] = str[start + c];    

    target[c] = '\0';
}

// int available_memory()
// {
//     int size = MEMORY;
//     byte *buf;
//     while ((buf = (byte *) malloc(--size)) == NULL);
//     free(buf);

//     return size; 
// }
