#pragma once
#include "UltrasonicSensor.h"
#include <Adafruit_MCP23X17.h>

class TDSCorrector
{
private:
    Adafruit_MCP23X17 _mcp;
    UltrasonicSensor _levelASensor;
    UltrasonicSensor _levelBSensor;
    bool _warningStatus;
    float _ATankVolume;
    float _BTankVolume;
    float _ATankMaxLevel;
    float _BTankMaxLevel;
    float _ATankMinLevel;
    float _BTankMinLevel;
    float _ATankHeight;
    float _BTankHeight;
    byte _pinPump;

public:
    TDSCorrector(Adafruit_MCP23X17 mcp);
    void loop();
    void setup();
    void activePump(float duration);
    float getALevelCm();
    float getBLevelCm();
    float getACurrentVolume();
    float getBCurrentVolume();
    bool isWarning();
};