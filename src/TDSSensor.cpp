#include "TDSSensor.h"

TDSSensor::TDSSensor(Adafruit_ADS1115 &ads, byte pin) : _ads(ads), _pin(pin) {}

float TDSSensor::readTDS(float temp)
{
    int16_t adcValue = _ads.readADC_SingleEnded(_pin);

    float voltage = _ads.computeVolts(adcValue);
    float compensationCoefficient = 1.0 + 0.02 * (temp - 25.0);
    float compensatedVoltage = voltage / compensationCoefficient;
    float tdsValue = (compensatedVoltage * 130.0) * _kValue;

    return tdsValue;
}