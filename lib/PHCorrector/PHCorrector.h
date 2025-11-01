#pragma once
#include "UltrasonicSensor.h"

class PHCorrector
{
private:
    UltrasonicSensor _levelPhUpSensor;
    UltrasonicSensor _levelPhDownSensor;
    byte _pinPumpPhUp;
    byte _pinPumpPhDown;
    float _phUpTankHeight;
    float _phDownTankHeight;

public:
    PHCorrector(
        byte pinTrigPhUpLevel,
        byte pinEchoPhUpLevel,
        byte pinTrigPhDownLevel,
        byte pinEchoPhDownLevel,
        byte pinPumpPhUp,
        byte pinPumpPhDown,
        float phUpTankHeight,
        float phDownTankHeight);
    void setup();
    void activePhUpPump(float duration);
    void activePhDownPump(float duration);
    float getPhUpLevelCm();
    float getPhDownLevelCm();
};