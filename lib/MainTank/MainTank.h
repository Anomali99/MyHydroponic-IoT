#pragma once
#include "UltrasonicSensor.h"

class MainTank
{
private:
    UltrasonicSensor _levelSensor;
    float _tankHeight;
    float _minLevel;
    float _maxLevel;
    byte _pinValve;
    byte _pinMixer;

public:
    MainTank(
        byte pinTrigLevel,
        byte pinEchoLevel,
        byte pinValve,
        byte pinMixer,
        float tankHeight,
        float minLevel,
        float maxLevel);
    void setup();
    void mix();
    float getLevelCm();
};