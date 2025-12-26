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
    _ads.setGain(GAIN_TWOTHIRDS);
    _tempSensor.setup();
    _mcp.pinMode(_valvePin, OUTPUT);
    _mcp.pinMode(_pumpPin, OUTPUT);
    _mcp.digitalWrite(_valvePin, HIGH);
    _mcp.digitalWrite(_pumpPin, HIGH);
}

void MeasuringTank::loop()
{
    static unsigned long lastKeepAlive = 0;
    static unsigned long sampleIndex = 0;
    unsigned long now = millis();

    if (now - lastKeepAlive > 100)
    {
        lastKeepAlive = now;

        switch (_statusState)
        {
        case FILL_TANK:
        {
            _mcp.digitalWrite(_pumpPin, LOW);
            break;
        }

        case CLEAN_TANK:
        {
            _mcp.digitalWrite(_valvePin, LOW);
            break;
        }

        default:
        {
            _mcp.digitalWrite(_pumpPin, HIGH);
            _mcp.digitalWrite(_valvePin, HIGH);
            break;
        }
        }
    }

    switch (_statusState)
    {
    case MEASURING_IDLE:
        break;

    case FILL_TANK:
        if (now - _lastTimeActivate >= _pumpDuration)
        {
            _mcp.digitalWrite(_pumpPin, HIGH);
            _statusState = WAIT_STABLE;
        }
        break;

    case WAIT_STABLE:
        if (now - _lastTimeActivate >= _stabilizeDuration)
        {
            sampleIndex = 0;
            _sampleData.clear();
            _lastTimeActivate = now;
            _statusState = READ_ENV;
        }
        break;

    case READ_ENV:
        if (sampleIndex < _sampleCount)
        {
            if (now - _lastTimeActivate >= 100)
            {
                EnvironmentData data;
                data.temp = _tempSensor.readTempC();
                data.ph = _phSensor.readPh();
                data.tds = _tdsSensor.readTDS(data.temp);

                if (sampleIndex >= 10 || _sampleCount <= 10)
                {
                    _sampleData.push_back(data);
                }

                _lastTimeActivate = now;
                sampleIndex++;
            }
        }
        else
        {
            float phCount = 0;
            float tdsCount = 0;
            float tempCount = 0;
            float dataCount = _sampleData.size();

            for (const EnvironmentData data : _sampleData)
            {
                phCount += data.ph;
                tdsCount += data.tds;
                tempCount += data.temp;
            }

            _envData.ph = phCount / dataCount;
            _envData.tds = tdsCount / dataCount;
            _envData.temp = tempCount / dataCount;

            _sampleData.clear();

            _mcp.digitalWrite(_valvePin, LOW);
            _lastTimeActivate = now;
            _statusState = CLEAN_TANK;
        }
        break;

    case CLEAN_TANK:
        if (now - _lastTimeActivate >= _valveDuration)
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
