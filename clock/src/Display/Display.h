#include <Arduino.h>
// enum DisplayBrightnessLevel : int
// {
//   ULTRALOW = 0x0,
//   VERYLOW = 0x1,
//   LOWER = 0x2,
//   DIM = 0x3,
//   Normal = 0x4,
//   BRIGHT = 0x5,
//   BRIGHTER = 0x6,
//   ULTRABRIGHT = 0x7
// };

class Display
{
public:
  void DisplayNumber(unsigned int toDisplay, char padChar, bool primary);
  void DisplayText(String toDisplay, bool primary);
  void ChangeBrightness(int brightness);
  void Setup();
  Display();
  int BrightnessLevel;

private:
  unsigned int DisplayBrightness(int number);
  unsigned int DisplayBrightnessSecond(int number);
  void Init();
  void DisplayNumberInternal(String toDisplay, bool primary);
};
