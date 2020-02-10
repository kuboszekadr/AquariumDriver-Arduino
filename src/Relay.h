#ifndef Relay_h
#define Relay_h

#include <Arduino.h>

class Relay
{
public:
  Relay(int pin);

  void turnOn();
  void turnOff();

  int pin();

private:
  int _pin;
};

#endif