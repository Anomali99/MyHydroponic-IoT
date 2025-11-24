#pragma once
#include <Arduino.h>

class InterruptButton
{
private:
    byte _pin;
    bool _btnPressed;
    unsigned long _lastPress;

public:
    InterruptButton(byte pin);
    void setup();
    void update();
    bool isBtnPressed();
    unsigned long getLastPress();
    void setBtnPressed(bool status);
    void setLastPress(unsigned long time);
};