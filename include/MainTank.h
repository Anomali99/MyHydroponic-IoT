#pragma once
#include "UltrasonicSensor.h"
#include <Adafruit_MCP23X17.h>

class MainTank
{
private:
    const unsigned long NO_CHANGE_TIMEOUT;
    const float LEVEL_INCREASE_THRESHOLD;
    Adafruit_MCP23X17 _mcp;
    UltrasonicSensor _levelSensor;
    bool _warningStatus;
    float _tankVolume;
    float _tankHeight;
    float _minLevel;
    float _maxLevel;
    byte _pinValve;
    byte _pinMixer;

public:
    MainTank(Adafruit_MCP23X17 mcp);
    void setup();
    void loop();
    void activeMixer();
    float getLevelCm();
    float getCurrentVolume();
    bool isWarning();
};