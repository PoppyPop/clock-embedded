#include "Eeprom24C32_64.h"

class Storage
{
  public:
    Storage(bool debug = false);
    void Setup();
    byte Read(unsigned int address);
    void Write(unsigned int address, byte data);

  private:
    Eeprom24C32_64 *eeprom;
    bool _debug;
};