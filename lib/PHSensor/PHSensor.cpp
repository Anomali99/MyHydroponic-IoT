#include "PHSensor.h"

PHSensor::PHSensor(Adafruit_ADS1115 ads, byte pin) : _ads(ads), _pin(pin) {}

float PHSensor::readPh()
{
    int16_t adcValue = _ads.readADC_SingleEnded(_pin);

    float voltage = adcValue * 6.144 / 32767.0;
    float phValue = 7.0 + ((PH_OFFSET - voltage) / PH_SLOPE);

    return phValue;
}