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
  void DisplayNumber(unsigned int toDisplay, char padChar, bool primary);
  void DisplayText(String toDisplay, bool primary);
  void ChangeBrightness(int brightness);
  void Setup(int dataPin, int clkPin, int csPin);
  Display2();
  int BrightnessLevel;

private:
  LedControl *lc;
  void DisplayInternal(String toDisplay, bool primary);
};
