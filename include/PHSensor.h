#pragma once
#include <Arduino.h>
#include <Adafruit_ADS1X15.h>

class PHSensor
{
private:
    const float _ph4Voltage = 4.35;
    const float _ph7Voltage = 3.03;
    const float _ph4Ref = 4.01;
    const float _ph7Ref = 6.86;
    float _phSlope;
    float _phIntercept;
    Adafruit_ADS1115 &_ads;
    byte _pin;

public:
    PHSensor(Adafruit_ADS1115 &ads, byte pin);
    float readVoltage();
    float readPh();
};