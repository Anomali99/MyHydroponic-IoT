#pragma once
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

class TemperatureSensor
{
private:
    const int _oneWirePin;
    OneWire _oneWire;
    DallasTemperature _sensors;
    DeviceAddress _sensorAddress;

public:
    TemperatureSensor(byte oneWirePin);
    void setup();
    float readTempC();
};