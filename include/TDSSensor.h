#pragma once
#include <Adafruit_ADS1X15.h>
#include <Arduino.h>

class TDSSensor
{
private:
    float _kValue = 0.38;
    Adafruit_ADS1115 &_ads;
    byte _pin;

public:
    TDSSensor(Adafruit_ADS1115 &ads, byte pin);
    float readTDS(float temp = 25.0);
};