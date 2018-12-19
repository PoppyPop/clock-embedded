#include <Arduino.h>
#include <Wire.h>
#include <TimeLib.h>
#include "RTC/RTC.h"
#include "Display/Display2.h"
#include "Network/Network.h"
#include "Temp/Temp.h"
#include "Adafruit_TPA2016.h"
#include "Storage/Storage.h"

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

void reReadRtc()
{
  UpdateClock();
}
// /Clock

// Storage
Storage storage;

#define BRIGHTNESS_STORAGE 0x00

bool brightChanged = false;

// /Storage

// 1-Wire network
Network network;
// End 1-Wire network

// Temp

Temp temp;

void DefaultSecondary()
{
  if (secondaryDefaultPersistant)
  {
    localText = String(round(temp.ReadTemp() * 10) / 10) + "+";
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
Display2 display;

void SetText(String text)
{
  textTimeout = (millis() / 1000) + 5;
  localDot = -1;
  localText = text;
}

void SetBrightness(bool value)
{
  display.ChangeBrightness(display.BrightnessLevel + ((value) ? 1 : -1));
  SetText(String(display.BrightnessLevel));
  brightChanged = true;
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

void SaveBrightness()
{
  if (brightChanged)
  {
    //Serial.println("Save");
    storage.Write(BRIGHTNESS_STORAGE, display.BrightnessLevel);
    brightChanged = false;
  }
}

void setup()
{
  Serial.begin(9600); // start serial for output

  Serial.println("Network");
  // Network
  network.Setup();

  Serial.println("Clock");
  // Clock
  Clock.InitClock(TimeZone);
  setSyncProvider(UpdateClock);

  Serial.println("Storage");
  // Storage
  storage.Setup();

  int readBright = storage.Read(BRIGHTNESS_STORAGE);

  Serial.print("Read brightness = ");
  Serial.println(readBright);

  Serial.println("Display");
  // Display
  //  pin 10 (12) is connected to the DataIn
  //  pin 11 is connected to the CLK
  //  pin 12 (10) is connected to LOAD
  display.Setup(10, 11, 12);
  display.ChangeBrightness(readBright);

  Serial.println("Audio");
  // Audio
  audioamp.begin();

  // Turn off AGC for the gain test
  //audioamp.setAGCCompression(TPA2016_AGC_OFF);
  // we also have to turn off the release to really turn off AGC
  //audioamp.setReleaseControl(0);

  //audioamp.setGain(9);

  Serial.println("Temp");
  // Temp
  temp.Setup(2, 5U);

  Serial.println("Network Callback");
  // Network Callback
  network.SetBrightnessCallback(SetBrightness);
  network.SetTextCallback(SetText);
  network.SwitchPersistantSecondary(SwitchPersistantSecondary);
  network.RereadRTC(reReadRtc);

  Serial.println("Ready!");

  //display.DisplayText("1234", false, 3);
}

void loop()
{
  display.DisplayInt(hour() * 100 + minute(), '0', true);
  display.DisplayText(GetDisplayText(), false, localDot);
  SaveBrightness();
}