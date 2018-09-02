
#include <DHT.h>
#include <DHT_U.h>

class Temp
{
  public:
    Temp();
    void Setup(int pin);
    float ReadTemp();
    float readHumidity();

  private:
    DHT_Unified *dht;
};