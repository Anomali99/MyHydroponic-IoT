#include "PHSensor.h"

PHSensor::PHSensor(byte pin) : _pin(pin) {}

void PHSensor::setup()
{
    pinMode(_pin, INPUT);
}

float PHSensor::readPh()
{
    long totalValue = 0;
    for (int i = 0; i < SAMPLES; i++)
    {
        totalValue += analogRead(_pin);
        delay(20);
    }

    float averageValue = totalValue / (float)SAMPLES;
    float voltage = (averageValue * VREF) / ADC_RESOLUTION;
    float phValue = (PH_SLOPE * voltage) + PH_OFFSET;

    return phValue;
}