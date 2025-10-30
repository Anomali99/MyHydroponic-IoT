#pragma once
#include "UltrasonicSensor.h"

class PHCorrector
{
private:
    UltrasonicSensor _levelPhUpSensor;
    UltrasonicSensor _levelPhDownSensor;
    float _maxLevelPhUp;
    float _maxLevelPhDown;
    byte _pinPumpPhUp;
    byte _pinPumpPhDown;

public:
    PHCorrector(
        byte pinTrigPhUpLevel,
        byte pinEchoPhUpLevel,
        byte pinTrigPhDownLevel,
        byte pinEchoPhDownLevel,
        byte pinPumpPhUp,
        byte pinPumpPhDown,
        float maxLevelPhUp,
        float maxLevelPhDown);
    void setup();
    void activePhUpPump(float duration);
    void activePhDownPump(float duration);
    float getPhUpLevelCm();
    float getPhDownLevelCm();
};