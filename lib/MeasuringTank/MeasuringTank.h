#pragma once
#include "PHSensor.h"
#include "TDSSensor.h"

struct EnvironmentData
{
    float ph;
    float tds;
};

class MeasuringTank
{
private:
    PHSensor _phSensor;
    TDSSensor _tdsSensor;
    byte _valvePin;
    byte _pumpPin;

public:
    MeasuringTank(
        byte phPin,
        byte tdsPin,
        byte valvePin,
        byte pumpPin);
    void setup();
    EnvironmentData readData();
};