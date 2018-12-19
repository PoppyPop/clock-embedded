#include <RTC/RTC.h>

RTC::RTC(bool debug)
{
    _debug = debug;
}

void RTC::UpdateTZ(int timezone)
{
    TimeZone = timezone;
}

void RTC::InitClock(int timezone)
{
    Clock = new RtcDS3231<TwoWire>(Wire);

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);

    if (!Clock->IsDateTimeValid())
    {
        // Common Cuases:
        //    1) first time you ran and the device wasn't running yet
        //    2) the battery on the device is low or even missing

        Serial.println("RTC lost confidence in the DateTime!");

        // following line sets the RTC to the date & time this sketch was compiled
        // it will also reset the valid flag internally unless the Rtc device is
        // having an issue

        Clock->SetDateTime(compiled);
    }

    if (!Clock->GetIsRunning())
    {
        Serial.println("RTC was not actively running, starting now");
        Clock->SetIsRunning(true);
    }

    RtcDateTime now = Clock->GetDateTime();
    if (now < compiled)
    {
        Serial.println("RTC is older than compile time!  (Updating DateTime)");
        //Clock->SetDateTime(compiled);
    }
    else if (now > compiled)
    {
        Serial.println("RTC is newer than compile time. (this is expected)");
    }
    else if (now == compiled)
    {
        Serial.println("RTC is the same as compile time! (not expected but all is fine)");
    }

    // never assume the Rtc was last configured by you, so
    // just clear them to your needed state
    Clock->Enable32kHzPin(false);
    Clock->SetSquareWavePin(DS3231SquareWavePin_ModeAlarmBoth);

    UpdateTZ(timezone);
}

time_t RTC::UpdateClock()
{
    RtcTemperature temp = Clock->GetTemperature();
    Serial.print("Inside temp: ");
    temp.Print(Serial);
    Serial.println();

    RtcDateTime base = Clock->GetDateTime();
    // By default add timezone (add 60seconds * 60 * Tz in hour)
    RtcDateTime future = base + (TimeZone * (60*60));

    bool dst = false;

    int thisMonth = future.Month();
    int thisDay = future.Day();
    int thisWeekday = future.DayOfWeek();
    int thisHour = future.Hour();

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

    // if dst, add one hour (add 60seconds * 60)
    if (dst)
        future = (future + (60*60));

    if (_debug)
        Serial.println("Date update !");

    tmElements_t tmSet;
    tmSet.Day = future.Day();
    tmSet.Hour = future.Hour();
    tmSet.Minute = future.Minute();
    tmSet.Month = future.Month();
    tmSet.Second = future.Second();
    tmSet.Wday = future.DayOfWeek();
    tmSet.Year = CalendarYrToTm(future.Year());

    return makeTime(tmSet);
}