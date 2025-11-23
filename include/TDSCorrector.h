#pragma once
#include "UltrasonicSensor.h"
#include <Adafruit_MCP23X17.h>

class TDSCorrector
{
private:
    Adafruit_MCP23X17 _mcp;
    UltrasonicSensor _levelASensor;
    UltrasonicSensor _levelBSensor;
    byte _pinPump;
    bool _warningStatus = false;
    float _ATankVolume = 1.0;
    float _BTankVolume = 1.0;
    float _ATankMaxLevel = 40.0;
    float _BTankMaxLevel = 40.0;
    float _ATankMinLevel = 10.0;
    float _BTankMinLevel = 10.0;
    float _ATankHeight = 60.0;
    float _BTankHeight = 60.0;

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