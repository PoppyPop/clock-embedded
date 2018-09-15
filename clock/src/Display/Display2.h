#include "LedControl.h"

const static bool reversed[8] PROGMEM = {
    false,
    false,
    true,
    true,
    false,
    false,
    false,
    false};

class Display2
{
public:
  void DisplayInt(unsigned int toDisplay, char padChar, bool primary, int dotPos = -1);
  void DisplayFloat(float toDisplay, char padChar, bool primary, int dotPos = -1);
  void DisplayText(String toDisplay, bool primary, int dotPos = -1);
  void ChangeBrightness(int brightness);
  void Setup(int dataPin, int clkPin, int csPin);
  Display2(bool debug = false);
  int BrightnessLevel = 5;

private:
  LedControl *lc;
  bool _debug;
  void DisplayInternal(String toDisplay, bool primary, int dotPos);
};
