#include <Storage/Storage.h>

#define EEPROM_ADDRESS 0x57

Storage::Storage(bool debug)
{
    _debug = debug;
}

void Storage::Setup(void)
{
    eeprom = new Eeprom24C32_64(EEPROM_ADDRESS);

    eeprom->initialize();
}

byte Storage::Read(unsigned int address)
{
    return eeprom->readByte(address);
}

void Storage::Write(unsigned int address, byte data)
{
    eeprom->writeByte(address, data);
}