#include "InterruptButton.h"

InterruptButton::InterruptButton(byte pin) : _pin(pin),
                                             _btnPressed(false),
                                             _lastPress(0)
{
}

void InterruptButton::setup()
{
    pinMode(_pin, INPUT_PULLUP);
}

void InterruptButton::update()
{
    if (digitalRead(_pin) == HIGH)
    {
        _btnPressed = true;
    }
}

bool InterruptButton::isBtnPressed()
{
    return _btnPressed;
}

unsigned long InterruptButton::getLastPress()
{
    return _lastPress;
}

void InterruptButton::setBtnPressed(bool status)
{
    _btnPressed = status;
}

void InterruptButton::setLastPress(unsigned long time)
{
    _lastPress = time;
}