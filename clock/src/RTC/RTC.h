#include <Arduino.h>
#include <Wire.h>
#include <TimeLib.h>
#include <RtcDS3231.h>

class RTC
{
public:
  RTC(bool debug = false);
  void UpdateTZ(int timezone);
  void InitClock(int timezone);
  time_t UpdateClock();

private:
  //RTC_DS3231 Clock;
  RtcDS3231<TwoWire> *Clock;
  int TimeZone;
  bool _debug;
};