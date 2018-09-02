#include <Temp/Temp.h>

#define DHTTYPE DHT22

Temp::Temp(void)
{
}

void Temp::Setup(int pin)
{
    dht = new DHT_Unified(pin, DHTTYPE);

    dht->begin();
}

float Temp::ReadTemp()
{
    sensors_event_t event;
    dht->temperature().getEvent(&event);
    if (isnan(event.temperature))
    {
        Serial.println("Error reading temperature!");
        return -1;
    }
    else
    {
        return event.temperature;
    }
}

float Temp::readHumidity()
{
    sensors_event_t event;
    dht->humidity().getEvent(&event);
    if (isnan(event.relative_humidity))
    {
        Serial.println("Error reading humidity!");
        return -1;
    }
    else
    {
        return event.relative_humidity;
    }
}