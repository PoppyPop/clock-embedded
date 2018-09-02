#include "Network/Network.h"
#include <Wire.h>

#define SLAVE_ADDRESS 0x12

// Event
void (*pBrightFunc)(bool action);
void Network::SetBrightnessCallback(void (*pFunc)(bool action))
{
    pBrightFunc = pFunc;
}

void (*pTextFunc)(String text);
void Network::SetTextCallback(void (*pFunc)(String text))
{
    pTextFunc = pFunc;
}

void (*pPersisFunc)();
void Network::SwitchPersistantSecondary(void (*pFunc)())
{
    pPersisFunc = pFunc;
}

void handleCommand(char command, String datas)
{
    Serial.println("Command: " + String(command) + " -> " + datas);

    switch (command)
    {
    case 'B':
        pBrightFunc((datas == "+"));
        break;
    case 'T':
        pTextFunc(datas);
        break;
    case 'P':
        pPersisFunc();
    default:
        Serial.println("Command unknown: " + String(command) and +" -> " + datas);
    }
}

void receiveData(int byteCount)
{
    // first byte = Command
    // other = datas
    bool first = true;
    char command;
    String datas = "";

    while (Wire.available())
    {
        int dataReceived = Wire.read();

        if (dataReceived < 0 || dataReceived > 127)
        {
            continue;
        }

        if (first)
        {
            command = char(dataReceived);
            first = false;
        }
        else
        {
            datas += char(dataReceived);
        }
    }

    handleCommand(command, datas);
}

void sendData()
{
    //int envoi = dataReceived + 1;
    Wire.write(1);
}

void Network::Setup(void)
{
    Wire.begin(SLAVE_ADDRESS);
    Wire.onReceive(receiveData);
    Wire.onRequest(sendData);
}

Network::Network(void)
{
}