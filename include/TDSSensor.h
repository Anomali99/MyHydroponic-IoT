#pragma once
#include <Adafruit_ADS1X15.h>
#include <Arduino.h>

class TDSSensor
{
private:
    int kValueAddress = 8;
    float kValue = 0.65;
    float tdsFactor = 0.64;
    Adafruit_ADS1115 &_ads;
    byte _pin;

public:
    TDSSensor(Adafruit_ADS1115 &ads, byte pin);
    float readVoltage();
    float readTDS(float temp = 25.0);
};