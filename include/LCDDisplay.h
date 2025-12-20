#pragma once
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "UltrasonicSensor.h"
#include "MeasuringTank.h"

struct EnvData
{
    float ph;
    float tds;
    float temp;
    float tankA;
    float tankB;
    float tankPhUp;
    float tankPhDown;
    float tankMain;
    String datetime;
};

class LCDDisplay
{
private:
    LiquidCrystal_I2C _lcd;
    int _scrollPosition = -1;
    String _dataMessage;
    String _datetime;
    String _getShortTime(const String &datetime);
    void _runText();

public:
    LCDDisplay(uint8_t lcdAddress);
    void setup();
    void loop();
    void clear();
    void showData(const EnvData &data);
    void showSetupData(const SetupData &setupData);
    void showMessage(const String &line1, const String &line2 = "");
};