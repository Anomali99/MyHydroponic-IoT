#include "TemperatureSensor.h"

TemperatureSensor::TemperatureSensor(byte oneWirePin)
    : _oneWirePin(oneWirePin),
      _oneWire(oneWirePin),
      _sensors(&_oneWire) {}

void TemperatureSensor::setup()
{
    _sensors.begin();
}

float TemperatureSensor::readTempC()
{
    _sensors.requestTemperatures();
    return _sensors.getTempCByIndex(0);
}