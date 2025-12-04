#pragma once
#include <Adafruit_MCP23X17.h>
#include <vector>
#include "PHSensor.h"
#include "TDSSensor.h"
#include "TemperatureSensor.h"

struct EnvironmentData
{
    float ph;
    float tds;
    float temp;
};

enum MeasuringState
{
    MEASURING_IDLE,
    FILL_TANK,
    READ_ENV,
    CLEAN_TANK,
};

class MeasuringTank
{
private:
    int _sampleCount = 10;
    MeasuringState _statusState = MEASURING_IDLE;
    unsigned long _lastTimeActivate = 0;
    Adafruit_ADS1115 &_ads;
    Adafruit_MCP23X17 &_mcp;
    PHSensor _phSensor;
    TDSSensor _tdsSensor;
    TemperatureSensor _tempSensor;
    std::vector<EnvironmentData> _sampleData;
    EnvironmentData _envData;
    byte _valvePin;
    byte _pumpPin;

public:
    MeasuringTank(
        Adafruit_ADS1115 &ads,
        Adafruit_MCP23X17 &mcp);
    void setup();
    void loop();
    void startReadData();
    MeasuringState getState();
    EnvironmentData getData();
};