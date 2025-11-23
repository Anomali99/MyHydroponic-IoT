#pragma once
#include "PHSensor.h"
#include "TDSSensor.h"
#include "TemperatureSensor.h"
#include "Config.h"
#include <Adafruit_MCP23X17.h>

struct EnvironmentData
{
    float ph;
    float tds;
    float temp;
};

class MeasuringTank
{
private:
    Adafruit_ADS1115 _ads;
    Adafruit_MCP23X17 _mcp;
    PHSensor _phSensor;
    TDSSensor _tdsSensor;
    TemperatureSensor _tempSensor;
    byte _valvePin;
    byte _pumpPin;

public:
    MeasuringTank(
        Adafruit_ADS1115 ads,
        Adafruit_MCP23X17 mcp);
    void setup();
    EnvironmentData readData();
};