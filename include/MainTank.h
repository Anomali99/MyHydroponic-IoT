#pragma once
#include <Adafruit_MCP23X17.h>
#include "UltrasonicSensor.h"

class MainTank
{
private:
    float _tankVolume = 22400.0; // ml
    float _tankHeight = 40.0;    // cm
    float _minLevel = 9.0;       // cm
    float _maxLevel = 20.0;      // cm
    const unsigned long NO_CHANGE_TIMEOUT = 300 * 1000;
    const float LEVEL_INCREASE_THRESHOLD = 0.5;
    unsigned long _lastTimeMixer = 0;
    unsigned long _lastTimeCheck = 0;
    unsigned long _mixerDuration = 30 * 1000;
    float _levelAtLastCheck = 0;
    bool _isAddingWater = false;
    bool _isActiveMixer = false;
    bool _warningStatus = false;
    Adafruit_MCP23X17 &_mcp;
    UltrasonicSensor _levelSensor;
    byte _pinValve;
    byte _pinMixer;

public:
    MainTank(Adafruit_MCP23X17 &mcp);
    void setup();
    void loop();
    void activeMixer();
    float getLevelCm();
    float getCurrentVolume();
    SetupData getSetupData();
    bool isWarning();
    bool isActiveMixer();
};