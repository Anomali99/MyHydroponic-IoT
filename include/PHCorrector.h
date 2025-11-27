#pragma once
#include "UltrasonicSensor.h"
#include <Adafruit_MCP23X17.h>

class PHCorrector
{
private:
    unsigned long _lastTimePump = 0;
    float _pumpDuration = 0;
    bool _isActivePump = false;
    bool _warningStatus = false;
    float _phUpTankVolume = 1.0;
    float _phDownTankVolume = 1.0;
    float _phUpTankMaxLevel = 40.0;
    float _phDownTankMaxLevel = 40.0;
    float _phUpTankMinLevel = 10.0;
    float _phDownTankMinLevel = 10.0;
    float _phUpTankHeight = 60.0;
    float _phDownTankHeight = 60.0;
    Adafruit_MCP23X17 &_mcp;
    UltrasonicSensor _levelPhUpSensor;
    UltrasonicSensor _levelPhDownSensor;
    byte _pinPumpPhUp;
    byte _pinPumpPhDown;

public:
    PHCorrector(Adafruit_MCP23X17 &mcp);
    void setup();
    void loop();
    void activePhUpPump(float duration);
    void activePhDownPump(float duration);
    float getPhUpLevelCm();
    float getPhDownLevelCm();
    float getPhUpCurrentVolume();
    float getPhDownCurrentVolume();
    bool isWarning();
    bool isActivePump();
};