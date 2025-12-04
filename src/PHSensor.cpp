#include "PHSensor.h"

PHSensor::PHSensor(Adafruit_ADS1115 &ads, byte pin) : _ads(ads), _pin(pin) {}

float PHSensor::readPh()
{
    int16_t adcValue = _ads.readADC_SingleEnded(_pin);
    float voltage = _ads.computeVolts(adcValue);
    float phValue = (_phSlope * voltage) + _phIntercept;

    return phValue;
}