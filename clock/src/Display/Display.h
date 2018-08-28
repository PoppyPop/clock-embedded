
enum DisplayBrightnessLevel : int
{
  ULTRALOW = 0x0,
  VERYLOW = 0x1,
  LOWER = 0x2,
  DIM = 0x3,
  Normal = 0x4,
  BRIGHT = 0x5,
  BRIGHTER = 0x6,
  ULTRABRIGHT = 0x7
};

class Display
{
public:
  void DisplayNumber(unsigned int toDisplay, unsigned int second);
  void ChangeBrightness(DisplayBrightnessLevel brightness);
  void Setup();
  Display();
};
