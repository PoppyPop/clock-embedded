#include "Display/Display2.h"

#define MaxBright 15

String primaryValue;
String secondaryValue;

static String padLeft(String base, char pad, unsigned int size)
{
    String result = "";

    base = base.substring(max((int)base.length() - (int)size, 0));

    for (unsigned int i = 0; i < size - base.length(); i++)
    {
        result.concat(pad);
    }
    result.concat(base);

    return result;
}

static String padLeft(unsigned int dec, char pad, unsigned int size)
{
    String base = String(dec);

    return padLeft(base, pad, size);
}

void Display2::DisplayNumber(unsigned int toDisplay, char padChar, bool primary)
{
    DisplayInternal(padLeft(toDisplay, padChar, 4), primary);
}

void Display2::DisplayText(String toDisplay, bool primary)
{
    DisplayInternal(padLeft(toDisplay, ' ', 4), primary);
}

void Display2::Setup(int dataPin, int clkPin, int csPin)
{

    lc = new LedControl(dataPin, clkPin, csPin, 1);

    /* Set common anode */
    lc->setCommonAnode(true);
    /* Go to normal mode */
    lc->shutdown(0, false);
    /* Set the brightness to a medium values */
    lc->setIntensity(0, 2);
    /* and clear the display */
    lc->clearDisplay(0);
}

Display2::Display2()
{
}

void Display2::ChangeBrightness(int brightness)
{
    if (brightness >= 0 && brightness < MaxBright)
    {
        lc->setIntensity(0, brightness);
        BrightnessLevel = brightness;
    }
}

void Display2::DisplayInternal(String toDisplay, bool primary)
{

    if (primary && toDisplay == primaryValue)
        return;
    if (!primary && toDisplay == secondaryValue)
        return;

    if (primary)
    {
        primaryValue = toDisplay;
    }
    else
    {
        secondaryValue = toDisplay;
    }

    int startIndex = (primary) ? 0 : 4;

    int charIndex = 0;

    for (int digit = startIndex; digit < startIndex + 4; digit++)
    {
        char value = toDisplay.charAt(charIndex);

        lc->setChar(0, digit, value, false);

        charIndex++;
    }
}
