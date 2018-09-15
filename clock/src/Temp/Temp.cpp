#include <Temp/Temp.h>

#define DHTTYPE DHT22

Temp::Temp(bool debug)
{
    _debug = debug;
}

void Temp::Setup(int pin, unsigned int interval)
{
    dht = new DHT_Unified(pin, DHTTYPE);

    dht->begin();

    sensor_t sensor;
    dht->temperature().getSensor(&sensor);

    unsigned int minDelay = sensor.min_delay / 1000U / 1000U;

    _interval = (interval < minDelay) ? minDelay : interval;

    if (_debug)
    {

        Serial.println("------------------------------------");
        Serial.println("Temperature");
        Serial.print("Sensor:       ");
        Serial.println(sensor.name);
        Serial.print("Driver Ver:   ");
        Serial.println(sensor.version);
        Serial.print("Unique ID:    ");
        Serial.println(sensor.sensor_id);
        Serial.print("Max Value:    ");
        Serial.print(sensor.max_value);
        Serial.println(" *C");
        Serial.print("Min Value:    ");
        Serial.print(sensor.min_value);
        Serial.println(" *C");
        Serial.print("Resolution:   ");
        Serial.print(sensor.resolution);
        Serial.println(" *C");
        Serial.println("------------------------------------");
        // Print humidity sensor details.
        dht->humidity().getSensor(&sensor);
        Serial.println("------------------------------------");
        Serial.println("Humidity");
        Serial.print("Sensor:       ");
        Serial.println(sensor.name);
        Serial.print("Driver Ver:   ");
        Serial.println(sensor.version);
        Serial.print("Unique ID:    ");
        Serial.println(sensor.sensor_id);
        Serial.print("Max Value:    ");
        Serial.print(sensor.max_value);
        Serial.println("%");
        Serial.print("Min Value:    ");
        Serial.print(sensor.min_value);
        Serial.println("%");
        Serial.print("Resolution:   ");
        Serial.print(sensor.resolution);
        Serial.println("%");
        Serial.println("------------------------------------");

        Serial.print("Min Delay (s): ");
        Serial.println(minDelay);

        Serial.print("Delay (s):     ");
        Serial.println(_interval);

        Serial.println("------------------------------------");
    }
}

void Temp::Read()
{
    unsigned long baseTime = millis() / 1000U;

    if (_lastRead == 0 || baseTime > _lastRead + _interval)
    {
        _lastRead = baseTime + 1;

        sensors_event_t event;
        dht->temperature().getEvent(&event);
        if (isnan(event.temperature))
        {
            Serial.println("Error reading temperature!");
            _Temp = -100;
        }
        else
        {
            _Temp = event.temperature;
        }
        dht->humidity().getEvent(&event);
        if (isnan(event.relative_humidity))
        {
            Serial.println("Error reading humidity!");
            _Hum = -100;
        }
        else
        {
            _Hum = event.relative_humidity;
        }

        if (_debug)
        {
            Serial.print("Temperature: ");
            Serial.print(_Temp);
            Serial.println(" *C");

            Serial.print("Humidity: ");
            Serial.print(_Hum);
            Serial.println("%");
        }
    }
}

float Temp::ReadTemp()
{
    Read();

    return _Temp;
}

float Temp::readHumidity()
{
    Read();

    return _Hum;
}