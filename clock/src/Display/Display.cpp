
#include "Display/Display.h"
#include <Arduino.h>

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

// unsigned int arraySegment[11];

struct CharMap
{
  char Char;
  unsigned int Value;
};

// . P
//     A
//    ___
// F | G  | B
//    ___
// E |    | C
//    ___
//     D
// * S

#define CharCode 11
CharMap arraySegment[CharCode] = {
    {'0', 0b1111110}, // A B C D E F
    {'1', 0b0110000}, // B c
    {'2', 0b1101101}, // A B D E G
    {'3', 0b1111001}, // A B C D G
    {'4', 0b0110011}, // B C F G
    {'5', 0b1011011}, // A C D F G
    {'6', 0b1011111}, // A C D E F G
    {'7', 0b1110000}, // A B C
    {'8', 0b1111111}, // A B C D E F G
    {'9', 0b1111011}, // A B C D F G
    {' ', 0b0000000}  // None = Off
};

CharMap arraySegmentReverse[CharCode] = {
    {'0', 0b1111110}, // A B C D E F
    {'1', 0b0000110}, // E F
    {'2', 0b1101101}, // A B D E G
    {'3', 0b1001111}, // A D E F G
    {'4', 0b0010111}, // C E F G
    {'5', 0b1011011}, // A C D F G
    {'6', 0b1111011}, // A B C D F G
    {'7', 0b0001110}, // D E F
    {'8', 0b1111111}, // A B C D E F G
    {'9', 0b1011111}, // A C D E F G
    {' ', 0b0000000}  // None = Off
};

CharMap *getElem(CharMap base[], char search)
{
  for (int x = 0; x < CharCode; x++)
  {
    if (search == base[x].Char)
    {
      return &base[x];
    }
  }
  return NULL;
}

unsigned int brightness[8];
DisplayBrightnessLevel BrightnessLevel;

// Define digit to light for display
// Order R ICBA

#define AddRecerse 16
#define AddDigit0 8
#define AddDigit1 4
#define AddDigit2 2
#define AddDigit3 1

unsigned int arrayDigit[9];

void Init()
{
  arrayDigit[0] = 0x8;  // 1000 INH
  arrayDigit[1] = 0x5;  //
  arrayDigit[2] = 0x7;  //
  arrayDigit[3] = 0x13; // 3 + reverse
  arrayDigit[4] = 0x11; // 1 + reverse
  arrayDigit[5] = 0x6;  //
  arrayDigit[6] = 0x0;  //
  arrayDigit[7] = 0x4;  //
  arrayDigit[8] = 0x2;  //

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
  brightness[0] = 1;
  brightness[1] = 5;
  brightness[2] = 50;
  brightness[3] = 200;
  brightness[4] = 500;
  brightness[5] = 1000;
  brightness[6] = 2000;
  brightness[7] = 5000;

  BrightnessLevel = DisplayBrightnessLevel::Normal;
}

unsigned int DisplayBrightness(int number)
{
  return brightness[BrightnessLevel] /* (1 - arraySegmentDelay[number])*/;
}

unsigned int DisplayBrightnessSecond(int number)
{
  return DisplayBrightness(number) * 0.5;
}

#define DIGIT_ON HIGH
#define DIGIT_OFF LOW

#define SEGMENT_ON LOW
#define SEGMENT_OFF HIGH

// set digit, return if digit is reversed
bool LightDigit(int digitToDisplay)
{

  digitalWrite(digit0, (arrayDigit[digitToDisplay] & AddDigit0) ? DIGIT_ON : DIGIT_OFF);
  digitalWrite(digit1, (arrayDigit[digitToDisplay] & AddDigit1) ? DIGIT_ON : DIGIT_OFF);
  digitalWrite(digit2, (arrayDigit[digitToDisplay] & AddDigit2) ? DIGIT_ON : DIGIT_OFF);
  digitalWrite(digit3, (arrayDigit[digitToDisplay] & AddDigit3) ? DIGIT_ON : DIGIT_OFF);

  return (arrayDigit[digitToDisplay] & AddRecerse);
}

//Given a number, turns on those segments
//If number == 10, then turn off number
void LightNumber(char numberToDisplay, bool dot, bool seconds, bool reverse)
{
  CharMap *digit = (reverse) ? arraySegmentReverse : arraySegment;

  digitalWrite(segA, (getElem(digit, numberToDisplay)->Value & AddA) ? SEGMENT_ON : SEGMENT_OFF);
  digitalWrite(segB, (getElem(digit, numberToDisplay)->Value & AddB) ? SEGMENT_ON : SEGMENT_OFF);
  digitalWrite(segC, (getElem(digit, numberToDisplay)->Value & AddC) ? SEGMENT_ON : SEGMENT_OFF);
  digitalWrite(segD, (getElem(digit, numberToDisplay)->Value & AddD) ? SEGMENT_ON : SEGMENT_OFF);
  digitalWrite(segE, (getElem(digit, numberToDisplay)->Value & AddE) ? SEGMENT_ON : SEGMENT_OFF);
  digitalWrite(segF, (getElem(digit, numberToDisplay)->Value & AddF) ? SEGMENT_ON : SEGMENT_OFF);
  digitalWrite(segG, (getElem(digit, numberToDisplay)->Value & AddG) ? SEGMENT_ON : SEGMENT_OFF);

  digitalWrite(segP, (dot) ? SEGMENT_ON : SEGMENT_OFF);
  digitalWrite(segS, (seconds) ? SEGMENT_ON : SEGMENT_OFF);
}

void DisplayNumberInternal(char toDisplay[], bool primary)
{
  int startIndex = (primary) ? 1 : 5;

  long beginTime = millis();
  int charIndex = 0;

  for (int digit = startIndex; digit < startIndex + 4; digit++)
  {
    char value = toDisplay[charIndex];

    //Turn on the right segments for this digit
    LightNumber(value, false, false, LightDigit(digit));

    delayMicroseconds(DisplayBrightness(value));
    //Display digit for fraction of a second (1us to 5000us, 500 is pretty good)

    //Turn off all segments
    LightNumber(' ', false, false, false);

    //Turn off all digits
    LightDigit(0);

    charIndex++;
  }

  while ((millis() - beginTime) < 10)
    ;
  //Wait for 20ms to pass before we paint the display again
}

void Display::DisplayNumber(int toDisplay, int second)
{
  char value[4];
  sprintf( value, "% 4d", toDisplay );
  DisplayNumberInternal(value, true);

  char secondary[4];
  sprintf( secondary, "% 4d", second );
  DisplayNumberInternal(secondary, false);
}

void Display::Setup()
{
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
}

Display::Display(void)
{
  Init();
}

void Display::ChangeBrightness(DisplayBrightnessLevel brightness)
{
  BrightnessLevel = brightness;
}
