#pragma once
#include "UltrasonicSensor.h"

class TDSCorrector
{
private:
    UltrasonicSensor _levelASensor;
    UltrasonicSensor _levelBSensor;
    float _ATankHeight;
    float _BTankHeight;
    byte _pinPump;

public:
    TDSCorrector(
        byte pinTrigALevel,
        byte pinEchoALevel,
        byte pinTrigBLevel,
        byte pinEchoBLevel,
        byte pinPump,
        float ATankHeight,
        float BTankHeight);
    void setup();
    void activePump(float duration);
    float getALevelCm();
    float getBLevelCm();
};