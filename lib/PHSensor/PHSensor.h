#pragma once
#include <Arduino.h>

#define VREF 3.3
#define ADC_RESOLUTION 4096.0
#define SAMPLES 30
#define PH_SLOPE -5.70
#define PH_OFFSET 21.34

class PHSensor
{
private:
    byte _pin;

public:
    PHSensor(byte pin);
    void setup();
    float readPh();
};