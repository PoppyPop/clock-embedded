#include <Arduino.h>
#include <Wire.h>
#include <TimeLib.h>
#include "RTC/RTC.h"
#include "Display/display.h"
#include "Network/Network.h"

int TimeZone = 1;

// Clock
RTC Clock;
time_t UpdateClock()
{
  return Clock.UpdateClock();
}
// /Clock

// 1-Wire network
Network network;
// End 1-Wire network

// Display
Display display;
void SetBrightness(bool value)
{
  display.ChangeBrightness(display.BrightnessLevel + ((value) ? 1 : -1));
}
// End Display

void setup()
{
  Serial.begin(9600); // start serial for output

  // Network
  network.Setup();
  network.SetBrightnessCallback(SetBrightness);

  // Clock
  Clock.InitClock(TimeZone);
  setSyncProvider(UpdateClock);

  // Display
  display.Setup();
  display.ChangeBrightness(4);

  Serial.println("Ready!");
}

void loop()
{
  display.DisplayNumber(hour() * 100 + minute(), (millis() / 1000) /*+9980*/);
}