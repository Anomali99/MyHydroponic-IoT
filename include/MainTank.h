#pragma once
#include <Adafruit_MCP23X17.h>
#include "UltrasonicSensor.h"

class MainTank
{
private:
    const unsigned long NO_CHANGE_TIMEOUT = 10000;
    const float LEVEL_INCREASE_THRESHOLD = 0.5;
    unsigned long _lastTimeMixer = 0;
    unsigned long _lastTimeCheck = 0;
    float _levelAtLastCheck = 0;
    long _mixerDuration = 3000;
    bool _isAddingWater = false;
    bool _isActiveMixer = false;
    bool _warningStatus = false;
    float _tankVolume = 2.0;
    float _tankHeight = 60.0;
    float _minLevel = 10.0;
    float _maxLevel = 40.0;
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
    bool isWarning();
    bool isActiveMixer();
};