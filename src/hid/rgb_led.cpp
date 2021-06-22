#include "hid/rgb_led.h"
using namespace daisy;
void RgbLed::Init(Pin red,
                  Pin green,
                  Pin blue,
                  bool         invert)
{
    r_.Init(red, invert);
    g_.Init(green, invert);
    b_.Init(blue, invert);
}

void RgbLed::Set(float r, float g, float b)
{
    r_.Set(r);
    g_.Set(g);
    b_.Set(b);
}

void RgbLed::SetColor(Color c)
{
    r_.Set(c.Red());
    g_.Set(c.Green());
    b_.Set(c.Blue());
}


void RgbLed::Update()
{
    r_.Update();
    g_.Update();
    b_.Update();
}
