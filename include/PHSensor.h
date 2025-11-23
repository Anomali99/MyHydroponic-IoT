#pragma once
#include <Arduino.h>
#include <Adafruit_ADS1X15.h>

#define PH_SLOPE 0.175
#define PH_OFFSET 2.5

class PHSensor
{
private:
    Adafruit_ADS1115 _ads;
    byte _pin;

public:
    PHSensor(Adafruit_ADS1115 ads, byte pin);
    float readPh();
};