#include "Utils.h"

float avg(const float *arr, const int arr_size)
{
    float sum = 0.0;

    for (int i = 0; i < arr_size; i++)
    {
        sum += arr[i];  
    }

    return sum / (arr_size*1.0);
}

void substring(char *target, const char *str, int start, int length)
{
    int c = 0;
    while (c < length)
    {
        target[c] = str[start + c];
        c++;    
    }

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
