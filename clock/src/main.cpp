#include <Arduino.h>
#include <Wire.h>
#include <TimeLib.h>
#include "RTC/RTC.h"
#include "Display/display.h"
#include "Display/Display2.h"
#include "Network/Network.h"
#include "Temp/Temp.h"
#include "Adafruit_TPA2016.h"

int TimeZone = 1;
String localText;
long textTimeout;
bool secondaryDefaultPersistant = true;

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

// Temp

Temp temp;

void DefaultSecondary()
{
  if (secondaryDefaultPersistant)
  {
    localText = String(round(temp.ReadTemp() * 10) / 10);
  }
  else
  {
    localText = String(round(temp.readHumidity() * 10) / 10);
  }
}
// /Temp

// Display
Display display;
Display2 display2;

void SetBrightness(bool value)
{
  display.ChangeBrightness(display.BrightnessLevel + ((value) ? 1 : -1));
}

void SetText(String text)
{
  localText = text;
  textTimeout = (millis() / 1000) + 5;
}

void SwitchPersistantSecondary()
{
  secondaryDefaultPersistant = !secondaryDefaultPersistant;
}

String GetDisplayText()
{
  long base = (millis() / 1000) - textTimeout;
  if (base >= 0 || base < -1000)
  {
    DefaultSecondary();
  }

  return localText;
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
  network.SwitchPersistantSecondary(SwitchPersistantSecondary);

  // Clock
  Clock.InitClock(TimeZone);
  setSyncProvider(UpdateClock);

  // Display
  display.Setup();
  display.ChangeBrightness(6);

  // Display2
  //  pin 12 is connected to the DataIn
  //  pin 11 is connected to the CLK
  //  pin 10 is connected to LOAD
  display2.Setup(12, 11, 10);
  display2.ChangeBrightness(6);

  // Audio
  audioamp.begin();

  // Turn off AGC for the gain test
  audioamp.setAGCCompression(TPA2016_AGC_OFF);
  // we also have to turn off the release to really turn off AGC
  audioamp.setReleaseControl(0);

  // Temp
  // Final : 2
  // Temp : 12
  temp.Setup(12);

  Serial.println("Ready!");
}

void loop()
{
  display.DisplayNumber(hour() * 100 + minute(), '0', true);
  display.DisplayText(GetDisplayText(), false);
}