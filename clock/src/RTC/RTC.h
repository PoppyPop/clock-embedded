#include <TimeLib.h>
#include "RTClib.h"

class RTC
{
public:
  void UpdateTZ(int timezone);
  void InitClock(int timezone);
  time_t UpdateClock();

private:
  RTC_DS3231 Clock;
  int TimeZone;
};