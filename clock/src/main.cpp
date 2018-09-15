#include <Arduino.h>
#include <Wire.h>
#include <TimeLib.h>
#include "RTC/RTC.h"
#include "Display/Display2.h"
#include "Network/Network.h"
#include "Temp/Temp.h"
#include "Adafruit_TPA2016.h"

int TimeZone = 1;
String localText;
int localDot = -1;
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
    localText = String(round(temp.ReadTemp() * 10) / 10) + "C";
    localDot = 3;
  }
  else
  {
    localText = String(round(temp.readHumidity() * 10) / 10) + "%";
    localDot = -1;
  }
}
// /Temp

// Display
#define SECONDS_PIN 5
Display2 display(true);

void SetBrightness(bool value)
{
  display.ChangeBrightness(display.BrightnessLevel + ((value) ? 1 : -1));
}

void SetText(String text)
{
  localText = text;
  localDot = -1;
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

  Serial.println("Network");
  // Network
  network.Setup();
  network.SetBrightnessCallback(SetBrightness);
  network.SetTextCallback(SetText);
  network.SwitchPersistantSecondary(SwitchPersistantSecondary);

  Serial.println("Clock");
  // Clock
  Clock.InitClock(TimeZone);
  setSyncProvider(UpdateClock);

  Serial.println("Display");
  // Display
  //  pin 10 (12) is connected to the DataIn
  //  pin 11 is connected to the CLK
  //  pin 12 (10) is connected to LOAD
  display.Setup(10, 11, 12);
  display.ChangeBrightness(15);

  Serial.println("Local Display");


  Serial.println("Audio");
  // Audio
  audioamp.begin();

  // Turn off AGC for the gain test
  audioamp.setAGCCompression(TPA2016_AGC_OFF);
  // we also have to turn off the release to really turn off AGC
  audioamp.setReleaseControl(0);

  Serial.println("Temp");
  // Temp
  temp.Setup(2, 5U);

  Serial.println("Ready!");

  //display.DisplayText("1234", false, 3);
}

void loop()
{
  display.DisplayInt(hour() * 100 + minute(), '0', true);
  display.DisplayText(GetDisplayText(), false, localDot);
}