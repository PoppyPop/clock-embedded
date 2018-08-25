#include "Network/Network.h"
#include <Wire.h>
#include <Arduino.h>

#define SLAVE_ADDRESS 0x12
int dataReceived = 0;

void receiveData(int byteCount)
{
    // first byte = Command
    // other = datas
    while (Wire.available())
    {
        dataReceived = Wire.read();
        Serial.print("Donnee recue : ");
        Serial.println(dataReceived);
    }
    Serial.println("End!");
}

void sendData()
{
    int envoi = dataReceived + 1;
    Wire.write(envoi);
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