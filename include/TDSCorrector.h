#pragma once
#include <Adafruit_MCP23X17.h>
#include "UltrasonicSensor.h"

class TDSCorrector
{
private:
    unsigned long _lastTimePump = 0;
    float _pumpDuration = 0;
    bool _isActivePump = false;
    bool _warningAStatus = false;
    bool _warningBStatus = false;
    float _ATankVolume = 1.0;
    float _BTankVolume = 1.0;
    float _ATankMaxLevel = 40.0;
    float _BTankMaxLevel = 40.0;
    float _ATankMinLevel = 10.0;
    float _BTankMinLevel = 10.0;
    float _ATankHeight = 60.0;
    float _BTankHeight = 60.0;
    Adafruit_MCP23X17 &_mcp;
    UltrasonicSensor _levelASensor;
    UltrasonicSensor _levelBSensor;
    byte _pinPump;

public:
    TDSCorrector(Adafruit_MCP23X17 &mcp);
    void loop();
    void setup();
    void activePump(float duration);
    float getALevelCm();
    float getBLevelCm();
    float getACurrentVolume();
    float getBCurrentVolume();
    bool isAWarning();
    bool isBWarning();
    bool isActivePump();
};