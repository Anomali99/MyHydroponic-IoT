#include "InterruptButton.h"

InterruptButton *InterruptButton::g_instance = nullptr;

void IRAM_ATTR InterruptButton::_staticBtnHandle()
{
    if (InterruptButton::g_instance != nullptr && digitalRead(InterruptButton::g_instance->_pin) == HIGH)
    {
        InterruptButton::g_instance->_btnPressed = true;
    }
}

InterruptButton::InterruptButton(byte pin) : _pin(pin),
                                             _btnPressed(false),
                                             _lastPress(0)
{
}

void InterruptButton::setup()
{
    InterruptButton::g_instance = this;

    pinMode(_pin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(_pin), InterruptButton::_staticBtnHandle, RISING);
}

bool InterruptButton::isBtnPressed()
{
    return _btnPressed;
}

long InterruptButton::getLastPress()
{
    return _lastPress;
}

void InterruptButton::setBtnPressed(bool status)
{
    _btnPressed = status;
}

void InterruptButton::setLastPress(long time)
{
    _lastPress = time;
}