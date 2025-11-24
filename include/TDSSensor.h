#pragma once

#include <Arduino.h>
#include <Adafruit_ADS1X15.h>

#define VREF 5.0
#define K_VALUE 1.0

class TDSSensor
{
private:
    Adafruit_ADS1115 &_ads;
    byte _pin;

public:
    TDSSensor(Adafruit_ADS1115 &ads, byte pin);
    float readTDS(float temp = 25.0);
};