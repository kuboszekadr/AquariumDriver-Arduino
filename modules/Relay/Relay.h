#ifndef Relay_h
#define Relay_h

#include "Arduino.h"
#include "Log.h"

class Relay
{
  public:
    Relay(int pin);

    void turn_on();
    void turn_off();

    int pin() {return _pin;};
  
  private:
    int _pin;
    char _log_msg[50];
};

#endif