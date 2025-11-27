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

void MeasuringTank::loop()
{
    unsigned long now = millis();

    switch (_statusState)
    {
    case MEASURING_IDLE:
        break;

    case FILL_TANK:
        if (now - _lastTimeActivate >= 5000)
        {
            _mcp.digitalWrite(_pumpPin, HIGH);
            _statusState = READ_ENV;
        }
        break;

    case READ_ENV:
        _envData.temp = _tempSensor.readTempC();
        _envData.ph = _phSensor.readPh();
        _envData.tds = _tdsSensor.readTDS(_envData.temp);

        _mcp.digitalWrite(_valvePin, LOW);
        _lastTimeActivate = now;
        _statusState = CLEAN_TANK;
        break;

    case CLEAN_TANK:
        if (now - _lastTimeActivate >= 5000)
        {
            _mcp.digitalWrite(_valvePin, HIGH);
            _statusState = MEASURING_IDLE;
        }
        break;
    }
}

void MeasuringTank::startReadData()
{
    if (_statusState == MEASURING_IDLE)
    {
        _mcp.digitalWrite(_pumpPin, LOW);
        _lastTimeActivate = millis();
        _statusState = FILL_TANK;
    }
}

MeasuringState MeasuringTank::getState()
{
    return _statusState;
}

EnvironmentData MeasuringTank::getData()
{
    return _envData;
}