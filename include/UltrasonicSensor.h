#pragma once
#include <Arduino.h>

class UltrasonicSensor
{
private:
    byte _pinTrig;
    byte _pinEcho;

public:
    UltrasonicSensor(byte pinTrig, byte pinEcho);
    void setup();
    float getDistanceCm();
};