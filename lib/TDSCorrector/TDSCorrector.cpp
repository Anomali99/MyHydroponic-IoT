#include "TDSCorrector.h"

TDSCorrector::TDSCorrector(
    byte pinTrigALevel,
    byte pinEchoALevel,
    byte pinTrigBLevel,
    byte pinEchoBLevel,
    byte pinPump,
    float ATankHeight,
    float BTankHeight) : _levelASensor(UltrasonicSensor(pinTrigALevel, pinEchoALevel)),
                         _levelBSensor(UltrasonicSensor(pinTrigBLevel, pinEchoBLevel)),
                         _pinPump(pinPump),
                         _ATankHeight(ATankHeight),
                         _BTankHeight(BTankHeight)
{
}

void TDSCorrector::setup()
{
    _levelASensor.setup();
    _levelBSensor.setup();
    pinMode(_pinPump, OUTPUT);
}

void TDSCorrector::activePump(float duration)
{
    digitalWrite(_pinPump, HIGH);
    delay(duration * 1000);
    digitalWrite(_pinPump, LOW);
}

float TDSCorrector::getALevelCm()
{
    float distance = _levelASensor.getDistanceCm();
    float level = _ATankHeight - distance;

    return level;
}

float TDSCorrector::getBLevelCm()
{
    float distance = _levelBSensor.getDistanceCm();
    float level = _BTankHeight - distance;

    return level;
}