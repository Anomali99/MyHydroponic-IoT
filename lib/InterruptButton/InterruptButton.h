#pragma once
#include <Arduino.h>

class InterruptButton
{
private:
    byte _pin;
    volatile bool _btnPressed;
    volatile unsigned long _lastPress;
    static void IRAM_ATTR _staticBtnHandle();

public:
    static InterruptButton *g_instance;
    InterruptButton(byte pin);
    void setup();
    bool isBtnPressed();
    long getLastPress();
    void setBtnPressed(bool status);
    void setLastPress(long time);
};