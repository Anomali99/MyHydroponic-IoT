#include "TDSSensor.h"

TDSSensor::TDSSensor(Adafruit_ADS1115 &ads, byte pin) : _ads(ads), _pin(pin) {}

float TDSSensor::readTDS(float temp)
{
    int16_t adcValue = _ads.readADC_SingleEnded(_pin);
    float voltage = _ads.computeVolts(adcValue);

    float ecValue = (133.42 * voltage * voltage * voltage - 255.86 * voltage * voltage + 857.39 * voltage) * kValue;
    float ecValue25 = ecValue / (1.0 + 0.02 * (temp - 25.0));
    float tdsValue = ecValue25 * tdsFactor;

    return tdsValue;
}