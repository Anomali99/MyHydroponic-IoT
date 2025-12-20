#pragma once
#include <Adafruit_MCP23X17.h>
#include "UltrasonicSensor.h"

class PHCorrector
{
private:
    float _phUpTankVolume = 377.14;   // ml
    float _phDownTankVolume = 377.14; // ml
    float _phUpTankHeight = 52.0;     // cm
    float _phDownTankHeight = 52.0;   // cm
    float _phUpTankMaxLevel = 30.0;   // cm
    float _phDownTankMaxLevel = 30.0; // cm
    float _phUpTankMinLevel = 9.0;    // cm
    float _phDownTankMinLevel = 9.0;  // cm
    unsigned long _lastTimePump = 0;
    unsigned long _pumpDuration = 0;
    bool _isActiveUpPump = false;
    bool _isActiveDownPump = false;
    bool _warningUpStatus = false;
    bool _warningDownStatus = false;
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
    SetupData getPhUpSetupData();
    SetupData getPhDownSetupData();
    bool isUpWarning();
    bool isDownWarning();
    bool isActivePump();
};