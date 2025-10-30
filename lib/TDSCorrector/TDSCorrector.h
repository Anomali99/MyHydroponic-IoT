#pragma once
#include "UltrasonicSensor.h"

class TDSCorrector
{
private:
    UltrasonicSensor _levelASensor;
    UltrasonicSensor _levelBSensor;
    float _maxLevelA;
    float _maxLevelB;
    byte _pinPump;

public:
    TDSCorrector(
        byte pinTrigALevel,
        byte pinEchoALevel,
        byte pinTrigBLevel,
        byte pinEchoBLevel,
        byte pinPump,
        float maxLevelA,
        float maxLevelB);
    void setup();
    void activePump(float duration);
    float getALevelCm();
    float getBLevelCm();
};