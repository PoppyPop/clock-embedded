#include <RTC/RTC.h>

void RTC::UpdateTZ(int timezone)
{
    TimeZone = timezone;
}

void RTC::InitClock(int timezone)
{
    if (!Clock.begin())
    {
        Serial.println("Couldn't find RTC");
        while (1)
            ;
    }

    if (Clock.lostPower())
    {
        Serial.println("RTC lost power, lets set the time!");
    }

    UpdateTZ(timezone);
}

time_t RTC::UpdateClock()
{

    DateTime base = Clock.now();
    // By default add timezone
    DateTime future(base + TimeSpan(0, TimeZone, 0, 0));

    bool dst = false;

    int thisMonth = future.month();
    int thisDay = future.day();
    int thisWeekday = future.dayOfTheWeek();
    int thisHour = future.hour();

    if (thisMonth == 10 && thisDay < (thisWeekday + 24))
    {
        dst = true;
    }

    if (thisMonth == 10 && thisDay > 24 && thisWeekday == 1 && thisHour < 2)
    {
        dst = true;
    }

    if (thisMonth < 10 && thisMonth > 3)
        dst = true;

    if (thisMonth == 3 && thisDay > 24 && thisDay >= (thisWeekday + 24))
    {
        if (!(thisWeekday == 1 && thisHour < 2))
            dst = true;
    }

    // if dst, add one hour
    if (dst)
        future = (future + TimeSpan(0, 1, 0, 0));

    Serial.println("Date update !");

    tmElements_t tmSet;
    tmSet.Day = future.day();
    tmSet.Hour = future.hour();
    tmSet.Minute = future.minute();
    tmSet.Month = future.month();
    tmSet.Second = future.second();
    tmSet.Wday = future.dayOfTheWeek();
    tmSet.Year = CalendarYrToTm(future.year());

    return makeTime(tmSet);
}