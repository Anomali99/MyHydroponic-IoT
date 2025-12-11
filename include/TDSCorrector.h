#pragma once
#include <Adafruit_MCP23X17.h>
#include "UltrasonicSensor.h"

class TDSCorrector
{
private:
    float _ATankVolume = 377.14; // ml
    float _BTankVolume = 377.14; // ml
    float _ATankHeight = 52.0;   // cm
    float _BTankHeight = 52.0;   // cm
    float _ATankMaxLevel = 30.0; // cm
    float _BTankMaxLevel = 30.0; // cm
    float _ATankMinLevel = 6.0;  // cm
    float _BTankMinLevel = 6.0;  // cm
    unsigned long _lastTimePump = 0;
    float _pumpDuration = 0;
    bool _isActivePump = false;
    bool _warningAStatus = false;
    bool _warningBStatus = false;
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