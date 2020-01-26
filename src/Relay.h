#ifndef Relay_h
#define Relay_h

#include <Arduino.h>

class Relay
{
  public:
    Relay(int pin);

    void turn_on();
    void turn_off();

    int pin();
  
  private:
    int _pin;
};

#endif