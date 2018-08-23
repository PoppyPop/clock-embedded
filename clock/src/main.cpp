#include <Arduino.h>
#include <Wire.h>
#include <TimeLib.h>
#include "RTC/RTC.h"
#include "Display/display.h"

int TimeZone = 1;

// Clock
RTC Clock;
time_t UpdateClock()
{
  return Clock.UpdateClock();
}

// /Clock

// 1-Wire network
#define SLAVE_ADDRESS 0x12
int dataReceived = 0;

void receiveData(int byteCount)
{
  while (Wire.available())
  {
    dataReceived = Wire.read();
    Serial.print("Donnee recue : ");
    Serial.println(dataReceived);
  }
}

void sendData()
{
  int envoi = dataReceived + 1;
  Wire.write(envoi);
}

// End 1-Wire network

// Display

Display display;

// End Display

void setup()
{
  Serial.begin(9600); // start serial for output

  // Network
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);

  // Clock
  Clock.InitClock(TimeZone);
  setSyncProvider(UpdateClock);

  // Display
  display.InitPin();
  display.ChangeBrightness(DisplayBrightnessLevel::LOWER);

  Serial.println("Ready!");
}

void loop()
{
  display.DisplayNumber(hour() * 100 + minute(), (millis() / 1000) + 1000);
}