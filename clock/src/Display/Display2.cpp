#include "Display/Display2.h"

#define MaxBright 15

String primaryValue;
int primaryDot;
String secondaryValue;
int secondaryDot;

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

void Display2::DisplayInt(unsigned int toDisplay, char padChar, bool primary, int dotPos)
{
    DisplayInternal(padLeft(toDisplay, padChar, 4), primary, dotPos);
}

void Display2::DisplayFloat(float toDisplay, char padChar, bool primary, int dotPos)
{
    DisplayInternal(padLeft(toDisplay, padChar, 4), primary, dotPos);
}

void Display2::DisplayText(String toDisplay, bool primary, int dotPos)
{
    DisplayInternal(padLeft(toDisplay, ' ', 4), primary, dotPos);
}

void Display2::Setup(int dataPin, int clkPin, int csPin)
{
    lc = new LedControl(dataPin, clkPin, csPin, 1);

    /* Set common anode */
    lc->setCommonAnode(true);
    /* Go to normal mode */
    lc->shutdown(0, false);
    /* Set the brightness to a medium values */
    lc->setIntensity(0, BrightnessLevel);
    /* and clear the display */
    lc->clearDisplay(0);
}

Display2::Display2(bool debug)
{
    _debug = debug;
}

void Display2::ChangeBrightness(int brightness)
{
    if (brightness >= 0 && brightness <= MaxBright)
    {
        lc->setIntensity(0, brightness);
        BrightnessLevel = brightness;
    }
}

void Display2::DisplayInternal(String toDisplay, bool primary, int dotPos)
{
    // TODO handle dot
    if (primary && toDisplay == primaryValue && dotPos == primaryDot)
        return;
    if (!primary && toDisplay == secondaryValue && dotPos == secondaryDot)
        return;

    if (primary)
    {
        primaryValue = toDisplay;
        primaryDot = dotPos;
    }
    else
    {
        secondaryValue = toDisplay;
        secondaryDot = dotPos;
    }

    int startIndex = (primary) ? 0 : 4;

    int charIndex = 0;

    for (int digit = startIndex; digit < startIndex + 4; digit++)
    {
        char value = toDisplay.charAt(charIndex);

        lc->setChar(0, digit, value, (charIndex == dotPos), pgm_read_byte_near(reversed + digit));

        charIndex++;
    }
}
