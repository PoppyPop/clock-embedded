#include <Arduino.h>
#include <Wire.h>
#include "RTClib.h"
#include <TimeLib.h>

RTC_DS3231 Clock;
bool Century = true;
bool h12;
bool PM;
byte ADay, AHour, AMinute, ASecond, ABits;
bool ADy, A12h, Apm;

int TimeZone = 1;

#define SLAVE_ADDRESS 0x12
int dataReceived = 0;

#define digit0 A0
#define digit1 A1
#define digit2 A2
#define digit3 A3

#define segA 7
#define segB 10
#define segC 3
#define segD 4
#define segE 6
#define segF 9
#define segG 2

#define segP 8
#define segS 5

// Define segments to light for display
// Order ABC DEFG

#define AddA 64
#define AddB 32
#define AddC 16
#define AddD 8
#define AddE 4
#define AddF 2
#define AddG 1

#define AddP 2
#define AddS 1

unsigned int arraySegment[11];
unsigned int arraySegmentReverse[11];
float arraySegmentDelay[11];

// Define digit to light for display
// Order R ICBA

#define AddRecerse 16
#define AddDigit0 8
#define AddDigit1 4
#define AddDigit2 2
#define AddDigit3 1

unsigned int arrayDigit[9];

void initDigits()
{

  // . P
  //     A
  //    ___
  // F | G  | B
  //    ___
  // E |    | C
  //    ___
  //     D
  // * S


  arraySegment[0] = 0x7E; // A B C D E F
  arraySegment[1] = 0x30; // B c
  arraySegment[2] = 0x6D; // A B D E G
  arraySegment[3] = 0x79; // A B C D G
  arraySegment[4] = 0x33; // B C F G
  arraySegment[5] = 0x5B; // A C D F G
  arraySegment[6] = 0x5F; // A C D E F G
  arraySegment[7] = 0x70; // A B C
  arraySegment[8] = 0x7F; // A B C D E F G
  arraySegment[9] = 0x7B; // A B C D F G
  arraySegment[10] = 0x00; // None = Off

  arraySegmentReverse[0] = 0x7E; // A B C D E F
  arraySegmentReverse[1] = 0x06; // E F
  arraySegmentReverse[2] = 0x6D; // A B D E G
  arraySegmentReverse[3] = 0x4F; // A D E F G
  arraySegmentReverse[4] = 0x17; // C E F G
  arraySegmentReverse[5] = 0x5B; // A C D F G
  arraySegmentReverse[6] = 0x7B; // A B C D F G
  arraySegmentReverse[7] = 0x0E; // D E F
  arraySegmentReverse[8] = 0x7F; // A B C D E F G
  arraySegmentReverse[9] = 0x5F; // A C D E F G
  arraySegmentReverse[10] = 0x00; // None = Off

  float baseDelay = 0.1;
  int baseFactor = 7;
  arraySegmentDelay[0] = (baseFactor - 6) * baseDelay; // A B C D E F
  arraySegmentDelay[1] = (baseFactor - 2) * baseDelay; // E F
  arraySegmentDelay[2] = (baseFactor - 5) * baseDelay; // A B D E G
  arraySegmentDelay[3] = (baseFactor - 5) * baseDelay; // A D E F G
  arraySegmentDelay[4] = (baseFactor - 4) * baseDelay; // C E F G
  arraySegmentDelay[5] = (baseFactor - 5) * baseDelay; // A C D F G
  arraySegmentDelay[6] = (baseFactor - 6) * baseDelay; // A B C D F G
  arraySegmentDelay[7] = (baseFactor - 3) * baseDelay; // D E F
  arraySegmentDelay[8] = (baseFactor - 7) * baseDelay; // A B C D E F G
  arraySegmentDelay[9] = (baseFactor - 6) * baseDelay; // A C D E F G
  arraySegmentDelay[10] = 0 * baseDelay; // None = OffOff

  arrayDigit[0] = 0x8;  // 1000 INH
  arrayDigit[1] = 0x5;  //
  arrayDigit[2] = 0x7;  //
  arrayDigit[3] = 0x13; // 3 + reverse
  arrayDigit[4] = 0x11; // 1 + reverse
  arrayDigit[5] = 0x6;  //
  arrayDigit[6] = 0x0;  //
  arrayDigit[7] = 0x4;  //
  arrayDigit[8] = 0x2;  //

}

//Display brightness
//Each digit is on for a certain amount of microseconds
//Then it is off until we have reached a total of 20ms for the function call
//Let's assume each digit is on for 1000us
//Each digit is on for 1ms, there are 4 digits, so the display is off for 16ms.
//That's a ratio of 1ms to 16ms or 6.25% on time (PWM).
//Let's define a variable called brightness that varies from:
//5000 blindingly bright (15.7mA current draw per digit)
//2000 shockingly bright (11.4mA current draw per digit)
//1000 pretty bright (5.9mA)
//500 normal (3mA)
//200 dim but readable (1.4mA)
//50 dim but readable (0.56mA)
//5 dim but readable (0.31mA)
//1 dim but readable in dark (0.28mA)
#define DISPLAY_BRIGHTNESS  300
#define DISPLAY_BRIGHTNESS_SECOND  DISPLAY_BRIGHTNESS*0.5

#define DIGIT_ON  HIGH
#define DIGIT_OFF  LOW

#define SEGMENT_ON  LOW
#define SEGMENT_OFF HIGH

// set digit, return if digit is reversed
bool lightDigit(int digitToDisplay) {

  digitalWrite(digit0, (arrayDigit[digitToDisplay] & AddDigit0) ? DIGIT_ON : DIGIT_OFF);
  digitalWrite(digit1, (arrayDigit[digitToDisplay] & AddDigit1) ? DIGIT_ON : DIGIT_OFF);
  digitalWrite(digit2, (arrayDigit[digitToDisplay] & AddDigit2) ? DIGIT_ON : DIGIT_OFF);
  digitalWrite(digit3, (arrayDigit[digitToDisplay] & AddDigit3) ? DIGIT_ON : DIGIT_OFF);

  return  (arrayDigit[digitToDisplay] & AddRecerse);

}

//Given a number, turns on those segments
//If number == 10, then turn off number
void lightNumber(int numberToDisplay, bool dot, bool seconds, bool reverse) {

  unsigned int* digit = (reverse) ? arraySegmentReverse : arraySegment;

  digitalWrite(segA, (digit[numberToDisplay] & AddA) ? SEGMENT_ON : SEGMENT_OFF);
  digitalWrite(segB, (digit[numberToDisplay] & AddB) ? SEGMENT_ON : SEGMENT_OFF);
  digitalWrite(segC, (digit[numberToDisplay] & AddC) ? SEGMENT_ON : SEGMENT_OFF);
  digitalWrite(segD, (digit[numberToDisplay] & AddD) ? SEGMENT_ON : SEGMENT_OFF);
  digitalWrite(segE, (digit[numberToDisplay] & AddE) ? SEGMENT_ON : SEGMENT_OFF);
  digitalWrite(segF, (digit[numberToDisplay] & AddF) ? SEGMENT_ON : SEGMENT_OFF);
  digitalWrite(segG, (digit[numberToDisplay] & AddG) ? SEGMENT_ON : SEGMENT_OFF);

  digitalWrite(segP, (dot) ? SEGMENT_ON : SEGMENT_OFF);
  digitalWrite(segS, (seconds) ? SEGMENT_ON : SEGMENT_OFF);

}

void displayNumber(int toDisplay, int second) {

  long beginTime = millis();

  for (int digit = 4 ; digit > 0 ; digit--) {

    //Turn on the right segments for this digit
    lightNumber(toDisplay % 10, false, false, lightDigit(digit));

    delayMicroseconds(DISPLAY_BRIGHTNESS * (1 - arraySegmentDelay[toDisplay % 10]));
    //Display digit for fraction of a second (1us to 5000us, 500 is pretty good)

    toDisplay /= 10;

    //Turn off all segments
    lightNumber(10, false, false, false);

    //Turn off all digits
    lightDigit(0);
  }

  for (int digit = 8 ; digit > 4 ; digit--) {

    //Turn on the right segments for this digit
    lightNumber(second % 10, false, false, lightDigit(digit));

    delayMicroseconds(DISPLAY_BRIGHTNESS_SECOND * (1 - arraySegmentDelay[second % 10]));
    //Display digit for fraction of a second (1us to 5000us, 500 is pretty good)

    second /= 10;

    //Turn off all segments
    lightNumber(10, false, false, false);

    //Turn off all digits
    lightDigit(0);
  }

  while ( (millis() - beginTime) < 10) ;
  //Wait for 20ms to pass before we paint the display again
}

time_t UpdateClock() {

DateTime base = Clock.now();
// By default add timezone
  DateTime future (base + TimeSpan(0, TimeZone, 0, 0));
  
  bool dst = false;

  int thisMonth = future.month();
  int thisDay = future.day();
  int thisWeekday = future.dayOfTheWeek();
  int thisHour = future.hour();
  
  if(thisMonth == 10 && thisDay < (thisWeekday + 24))
  {
    dst=true;
  }

  if(thisMonth == 10 && thisDay > 24 && thisWeekday == 1 && thisHour < 2)
  {
    dst=true;
  }

  if(thisMonth < 10 && thisMonth > 3) dst = true;
  
  if(thisMonth == 3 && thisDay > 24 && thisDay >= (thisWeekday + 24))
  {
    if(!(thisWeekday == 1 && thisHour < 2)) dst = true;
  }
  
  // if dst, add one hour
  if(dst) future = (future + TimeSpan(0, 1, 0, 0)); 
  
Serial.println("Date update !");

tmElements_t tmSet;
tmSet.Day = future.day();
tmSet.Hour = future.hour();
tmSet.Minute = future.minute();
tmSet.Month = future.month();
tmSet.Second = future.second();
tmSet.Wday = future.dayOfTheWeek();
tmSet.Year = future.year();

return makeTime(tmSet);


}


// 1-Wire network

void receiveData(int byteCount) {
  while (Wire.available()) {
    dataReceived = Wire.read();
    Serial.print("Donnee recue : ");
    Serial.println(dataReceived);
  }
}

void sendData() {
  int envoi = dataReceived + 1;
  Wire.write(envoi);
}

// End 1-Wire network

void setup() {
  initDigits();

  pinMode(segA, OUTPUT);
  pinMode(segB, OUTPUT);
  pinMode(segC, OUTPUT);
  pinMode(segD, OUTPUT);
  pinMode(segE, OUTPUT);
  pinMode(segF, OUTPUT);
  pinMode(segG, OUTPUT);

  pinMode(segP, OUTPUT);
  pinMode(segS, OUTPUT);

  pinMode(digit0, OUTPUT);
  pinMode(digit1, OUTPUT);
  pinMode(digit2, OUTPUT);
  pinMode(digit3, OUTPUT);

  Serial.begin(9600); // start serial for output
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);
  Serial.println("Ready!");


  if (! Clock.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (Clock.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
  }

  setSyncProvider(UpdateClock);
  
}

void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(hour());
  Serial.print(" ");
  Serial.print(minute());
  Serial.print(" ");
  Serial.print(second());
  Serial.println();
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 
}

void loop() {

//digitalClockDisplay();
  //displayNumber(1234, 5678);
  displayNumber(hour()*100+minute(), (millis() / 1000) + 1000);
}