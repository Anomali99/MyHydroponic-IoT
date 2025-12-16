#pragma once
#include <Arduino.h>

struct SetupData
{
    String name;
    float volume;
    float level;
    float distance;
};

class UltrasonicSensor
{
private:
    byte _pinTrig;
    byte _pinEcho;
    int _maxRetries = 10;

public:
    UltrasonicSensor(byte pinTrig, byte pinEcho);
    void setup();
    float getDistanceCm();
    float getSetupDistanceCm();
};