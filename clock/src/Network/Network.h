
#include <Arduino.h>

class Network
{
public:
  Network();
  void Setup();
  void SetBrightnessCallback(void (*pFunc)(bool action));
  void SetTextCallback(void (*pFunc)(String text));

private:

};