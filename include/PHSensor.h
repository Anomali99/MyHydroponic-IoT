#pragma once
#include <Arduino.h>
#include <Adafruit_ADS1X15.h>

class PHSensor
{
private:
    float _phSlope = -2.045;
    float _phIntercept = 12.736;
    Adafruit_ADS1115 &_ads;
    byte _pin;

public:
    PHSensor(Adafruit_ADS1115 &ads, byte pin);
    float readPh();
};