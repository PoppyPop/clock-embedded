#include "Network/Network.h"
#include <Wire.h>
#include <Arduino.h>

#define SLAVE_ADDRESS 0x12

void receiveData(int byteCount)
{
    // first byte = Command
    // other = datas
    bool first = true;
    String command;
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

    Serial.println("Command: " + command + " -> " + datas);
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