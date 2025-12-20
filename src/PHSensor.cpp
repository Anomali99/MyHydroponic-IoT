#include "PHSensor.h"

PHSensor::PHSensor(Adafruit_ADS1115 &ads, byte pin) : _ads(ads), _pin(pin)
{
    _phSlope = (_ph7Ref - _ph4Ref) / (_ph7Voltage - _ph4Voltage);
    _phIntercept = _ph7Ref - (_phSlope * _ph7Voltage);
}

float PHSensor::readVoltage()
{
    int16_t adcValue = _ads.readADC_SingleEnded(_pin);
    float voltage = _ads.computeVolts(adcValue);

    return voltage;
}

float PHSensor::readPh()
{
    float voltage = readVoltage();
    float phValue = (_phSlope * voltage) + _phIntercept;

    return phValue;
}