#include "TDSSensor.h"

TDSSensor::TDSSensor(byte pin) : _pin(pin) {}

void TDSSensor::setup()
{
    pinMode(_pin, INPUT);
}

float TDSSensor::readTDS(float temp)
{
    long totalValue = 0;
    for (int i = 0; i < SAMPLES; i++)
    {
        totalValue += analogRead(_pin);
        delay(40);
    }

    float averageValue = totalValue / (float)SAMPLES;
    float voltage = (averageValue * VREF) / ADC_RESOLUTION;
    float compensationCoefficient = 1.0 + 0.02 * (temp - 25.0);
    float compensationVoltage = voltage / compensationCoefficient;

    if (compensationVoltage < 0)
        compensationVoltage = 0;

    float tdsValue = (133.42 * pow(compensationVoltage, 3) - 255.86 * pow(compensationVoltage, 2) + 857.39 * compensationVoltage) * 0.5;

    return tdsValue;
}