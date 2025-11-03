#include "MeasuringTank.h"

MeasuringTank::MeasuringTank(
    byte phPin,
    byte tdsPin,
    byte valvePin,
    byte pumpPin) : _phSensor(phPin),
                    _tdsSensor(tdsPin),
                    _valvePin(valvePin),
                    _pumpPin(pumpPin) {}

void MeasuringTank::setup()
{
    _phSensor.setup();
    _tdsSensor.setup();
    pinMode(_valvePin, OUTPUT);
    pinMode(_pumpPin, OUTPUT);
}

EnvironmentData MeasuringTank::readData()
{
    EnvironmentData env;

    digitalWrite(_pumpPin, HIGH);
    delay(5000);
    digitalWrite(_pumpPin, LOW);

    env.ph = _phSensor.readPh();
    env.tds = _tdsSensor.readTDS();

    digitalWrite(_valvePin, HIGH);
    delay(5000);
    digitalWrite(_valvePin, LOW);

    return env;
}