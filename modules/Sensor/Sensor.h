#ifndef Sensor_h
#define  Sensor_h

#include "Arduino.h"
#include "RTC.h"
#include "Utils.h"
#include "Relay.h"

// send new data in approx every 30s
#define SAMPLING_INTERVAL 1000L // sample every second
#define SAMPLING_AMOUNT 30  // readings array size
#define RELAYS_AMONUT 3  // amount of relays connected to the sensor

struct Reading //: Printable
{
  int id_sensor;
  float value;
  char timestamp[17];

  // size_t printTo(Print &p) const
  // {
  //   char msg[100];
  //   strcpy(msg, "sensor:");
  //   strcat(msg, id_sensor);
  //   strcat(msg, " ");
    
  //   char _value[6];
  //   dtostrf(value, 2, 2, _value);
  //   strcat(msg, _value);

  //   return p.print(msg);
  // }
};

class Sensor
{
  public:
      virtual bool make_reading() = 0;

      Reading get_reading();

      void attach_relay(int pin);
      Relay *relay;

      bool available();
      bool ready();

  protected:
      float _avg();
      float _last_reading_value;

      float _readings[SAMPLING_AMOUNT];
      unsigned long _last_reading = 0;

      int _readings_count = -1;
      int _id_sensor = -1;
};

#endif  