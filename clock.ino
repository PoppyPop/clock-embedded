/*
 6-13-2011
 Spark Fun Electronics 2011
 Nathan Seidle
 
 This code is public domain but you buy me a beer if you use this and we meet 
 someday (Beerware license).
 
 4 digit 7 segment display:
 http://www.sparkfun.com/products/9483
 Datasheet: 
 http://www.sparkfun.com/datasheets/Components/LED/7-Segment/YSD-439AR6B-35.pdf

 This is an example of how to drive a 7 segment LED display from an ATmega
 without the use of current limiting resistors. This technique is very common 
 but requires some knowledge of electronics - you do run the risk of dumping 
 too much current through the segments and burning out parts of the display. 
 If you use the stock code you should be ok, but be careful editing the 
 brightness values.
 
 This code should work with all colors (red, blue, yellow, green) but the 
 brightness will vary from one color to the next because the forward voltage 
 drop of each color is different. This code was written and calibrated for the 
 red color.

 This code will work with most Arduinos but you may want to re-route some of 
 the pins.

 7 segments
 4 digits
 1 colon
 =
 12 pins required for full control 
 
 */

#include <Wire.h>

#define SLAVE_ADDRESS 0x12
int dataReceived = 0;

#define digit0 A0 //PWM Display pin 1
#define digit1 A3 //PWM Display pin 2
#define digit2 A2 //PWM Display pin 6
#define digit3 A1 //PWM Display pin 8

//Pin mapping from Arduino to the ATmega DIP28 if you need it
//http://www.arduino.cc/en/Hacking/PinMapping
#define segA 8 //Display pin 5
#define segB 10 //Display pin 7
#define segC 5 //Display pin 2
#define segD 6 //Display pin 3
#define segE 7 //Display pin 4
#define segF 11 //Display pin 8
#define segG 4 //Display pin 1

#define segP 9 //Display pin 6
#define segS 12 //Display pin X


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

  arrayDigit[0] = 0x8; 
  arrayDigit[1] = 0x0; 
  arrayDigit[2] = 0x1; 
  arrayDigit[3] = 0x12; 
  arrayDigit[4] = 0x13; 
  arrayDigit[5] = 0x4; 
  arrayDigit[6] = 0x5; 
  arrayDigit[7] = 0x6; 
  arrayDigit[8] = 0x7; 
  
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

#define DISPLAY_BRIGHTNESS  200

#define DIGIT_ON  HIGH
#define DIGIT_OFF  LOW

#define SEGMENT_ON  LOW
#define SEGMENT_OFF HIGH

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

  digitalWrite(digit1, DIGIT_ON);

  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  digitalWrite(segP, SEGMENT_OFF);
  digitalWrite(segS, SEGMENT_OFF);

  Serial.begin(9600); // start serial for output
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);
  Serial.println("Ready!");
  
}

void loop() { 
  displayNumber(millis()/1000, (millis()/1000)+1000); 
}

void displayNumber(int toDisplay, int second) {

  long beginTime = millis();

  for(int digit = 4 ; digit > 0 ; digit--) {

    //Turn on the right segments for this digit
    lightNumber(toDisplay % 10, lightDigit(digit));
    toDisplay /= 10;

    delayMicroseconds(DISPLAY_BRIGHTNESS); 
    //Display digit for fraction of a second (1us to 5000us, 500 is pretty good)

    //Turn off all segments
    lightNumber(10, false); 

    //Turn off all digits
    lightDigit(0);
  }

  for(int digit = 8 ; digit > 4 ; digit--) {

    //Turn on the right segments for this digit
    lightNumber(second % 10, lightDigit(digit));
    second /= 10;

    delayMicroseconds(DISPLAY_BRIGHTNESS); 
    //Display digit for fraction of a second (1us to 5000us, 500 is pretty good)

    //Turn off all segments
    lightNumber(10, false); 

    //Turn off all digits
    lightDigit(0);
  }

  while( (millis() - beginTime) < 10) ; 
  //Wait for 20ms to pass before we paint the display again
}

// set digit, return if digit is reversed
bool lightDigit(int digitToDisplay) {

  digitalWrite(digit0, (arrayDigit[digitToDisplay] & AddDigit0)?DIGIT_ON:DIGIT_OFF);
  digitalWrite(digit1, (arrayDigit[digitToDisplay] & AddDigit1)?DIGIT_ON:DIGIT_OFF);
  digitalWrite(digit2, (arrayDigit[digitToDisplay] & AddDigit2)?DIGIT_ON:DIGIT_OFF);
  digitalWrite(digit3, (arrayDigit[digitToDisplay] & AddDigit3)?DIGIT_ON:DIGIT_OFF);

  return  (arrayDigit[digitToDisplay] & AddRecerse);

}

//Given a number, turns on those segments
//If number == 10, then turn off number
void lightNumber(int numberToDisplay, bool reverse) {

  unsigned int* digit = (reverse)?arraySegmentReverse:arraySegment;

  digitalWrite(segA, (digit[numberToDisplay] & AddA)?SEGMENT_ON:SEGMENT_OFF);
  digitalWrite(segB, (digit[numberToDisplay] & AddB)?SEGMENT_ON:SEGMENT_OFF);
  digitalWrite(segC, (digit[numberToDisplay] & AddC)?SEGMENT_ON:SEGMENT_OFF);
  digitalWrite(segD, (digit[numberToDisplay] & AddD)?SEGMENT_ON:SEGMENT_OFF);
  digitalWrite(segE, (digit[numberToDisplay] & AddE)?SEGMENT_ON:SEGMENT_OFF);
  digitalWrite(segF, (digit[numberToDisplay] & AddF)?SEGMENT_ON:SEGMENT_OFF);
  digitalWrite(segG, (digit[numberToDisplay] & AddG)?SEGMENT_ON:SEGMENT_OFF);
}

void receiveData(int byteCount){
    while(Wire.available()) {
        dataReceived = Wire.read();
        Serial.print("Donnee recue : ");
        Serial.println(dataReceived);
    }
}

void sendData(){
    int envoi = dataReceived + 1;
    Wire.write(envoi);
}
