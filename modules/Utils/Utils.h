#ifndef Utils_h
#define  Utils_h

#define MEMORY 8192

#include <SoftwareSerial.h>
#include <Log.h>

// TODO: Add namespace
void clear(char *arr);
void read_serial(char *arr, int arr_size, SoftwareSerial *serial);
void print_serial(const char *arr, SoftwareSerial *serial);
void clear_buffer(SoftwareSerial *serial);
void substring(char *target, const char *str, int start, int length);

float avg(const float *arr, const int arr_size);

int available_memory();

#endif