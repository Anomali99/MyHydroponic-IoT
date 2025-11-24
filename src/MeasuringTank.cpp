#include "MeasuringTank.h"
#include "Config.h"

MeasuringTank::MeasuringTank(
    Adafruit_ADS1115 &ads,
    Adafruit_MCP23X17 &mcp) : _ads(ads),
                              _mcp(mcp),
                              _phSensor(_ads, MeT_PH_PIN),
                              _tdsSensor(_ads, MeT_TDS_PIN),
                              _tempSensor(MeT_TEMP_PIN),
                              _valvePin(MeT_VALVE_PIN),
                              _pumpPin(MeT_PUMP_PIN) {}

void MeasuringTank::setup()
{
    _tempSensor.setup();
    _mcp.pinMode(_valvePin, OUTPUT);
    _mcp.pinMode(_pumpPin, OUTPUT);
    _mcp.digitalWrite(_valvePin, HIGH);
    _mcp.digitalWrite(_pumpPin, HIGH);
}

EnvironmentData MeasuringTank::readData()
{
    EnvironmentData env;

    _mcp.digitalWrite(_pumpPin, LOW);
    delay(5000);
    _mcp.digitalWrite(_pumpPin, HIGH);

    env.temp = _tempSensor.readTempC();
    env.ph = _phSensor.readPh();
    env.tds = _tdsSensor.readTDS(env.temp);

    _mcp.digitalWrite(_valvePin, LOW);
    delay(5000);
    _mcp.digitalWrite(_valvePin, HIGH);

    return env;
}