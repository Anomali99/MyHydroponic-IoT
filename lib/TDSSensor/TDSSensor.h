#pragma once

#include <Arduino.h>

#define VREF 3.3
#define ADC_RESOLUTION 4096.0
#define SAMPLES 30

class TDSSensor
{
private:
    byte _pin;

public:
    TDSSensor(byte pin);
    void setup();
    float readTDS(float temp = 25.0);
};