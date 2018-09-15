
#include <DHT.h>
#include <DHT_U.h>

class Temp
{
  public:
    Temp(bool debug = false);
    void Setup(int pin, unsigned int interval = 5U * 60U);
    float ReadTemp();
    float readHumidity();

  private:
    DHT_Unified *dht;
    bool _debug;
    unsigned int _interval;
    unsigned long _lastRead;
    float _Temp;
    float _Hum;
    void Read();
};