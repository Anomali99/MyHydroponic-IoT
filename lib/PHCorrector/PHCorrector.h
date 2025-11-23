#pragma once
#include "UltrasonicSensor.h"
#include "Config.h"
#include <Adafruit_MCP23X17.h>

class PHCorrector
{
private:
    Adafruit_MCP23X17 _mcp;
    UltrasonicSensor _levelPhUpSensor;
    UltrasonicSensor _levelPhDownSensor;
    byte _pinPumpPhUp;
    byte _pinPumpPhDown;
    bool _warningStatus;
    float _phUpTankVolume;
    float _phDownTankVolume;
    float _phUpTankMaxLevel;
    float _phDownTankMaxLevel;
    float _phUpTankMinLevel;
    float _phDownTankMinLevel;
    float _phUpTankHeight;
    float _phDownTankHeight;

public:
    PHCorrector(Adafruit_MCP23X17 mcp);
    void setup();
    void loop();
    void activePhUpPump(float duration);
    void activePhDownPump(float duration);
    float getPhUpLevelCm();
    float getPhDownLevelCm();
    float getPhUpCurrentVolume();
    float getPhDownCurrentVolume();
    bool isWarning();
};