#include <Arduino.h>
#include <Wire.h>
#include <TimeLib.h>
#include "RTC/RTC.h"
#include "Display/display.h"
#include "Network/Network.h"
#include "Adafruit_TPA2016.h"

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
String localText;
void SetBrightness(bool value)
{
  display.ChangeBrightness(display.BrightnessLevel + ((value) ? 1 : -1));
}

void SetText(String text)
{
  // display.DisplayText(text, false);
  localText = text;
}
// End Display

// Audio
Adafruit_TPA2016 audioamp = Adafruit_TPA2016();

// /Audio

void setup()
{
  Serial.begin(9600); // start serial for output

  // Network
  network.Setup();
  network.SetBrightnessCallback(SetBrightness);
  network.SetTextCallback(SetText);

  // Clock
  Clock.InitClock(TimeZone);
  setSyncProvider(UpdateClock);

  // Display
  display.Setup();
  display.ChangeBrightness(6);

  // Audio
  audioamp.begin();

  // Turn off AGC for the gain test
  audioamp.setAGCCompression(TPA2016_AGC_OFF);
  // we also have to turn off the release to really turn off AGC
  audioamp.setReleaseControl(0);

  //audioamp.setNoiseGateThreshold(3);

  // OK now we'll turn the AGC back on and mess with the settings :)
  
  // AGC can be TPA2016_AGC_OFF (no AGC) or
  //  TPA2016_AGC_2 (1:2 compression)
  //  TPA2016_AGC_4 (1:4 compression)
  //  TPA2016_AGC_8 (1:8 compression)
  // Serial.println("Setting AGC Compression");
  //audioamp.setAGCCompression(TPA2016_AGC_8);

  // // See Datasheet page 22 for value -> dBV conversion table
  // Serial.println("Setting Limit Level");
  // audioamp.setLimitLevelOn();
  // // or turn off with setLimitLevelOff()
  // audioamp.setLimitLevel(0);  // range from 0 (-6.5dBv) to 31 (9dBV)
  
  // // See Datasheet page 23 for value -> ms conversion table
  // Serial.println("Setting AGC Attack");
  // audioamp.setAttackControl(5);
  
  // // See Datasheet page 24 for value -> ms conversion table
  // Serial.println("Setting AGC Hold");
  // audioamp.setHoldControl(0);
  
  // // See Datasheet page 24 for value -> ms conversion table
  // Serial.println("Setting AGC Release");
  // audioamp.setReleaseControl(11);

  // localText = "0000";

  Serial.println("Ready!");
}

void loop()
{
  display.DisplayNumber(hour() * 100 + minute(), '0', true);
  display.DisplayText(localText, false);
}