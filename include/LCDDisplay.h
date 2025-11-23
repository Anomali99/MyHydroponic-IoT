#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

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
    uint8_t _currentPage;
    const uint8_t _maxPages = 4;
    String _getShortTime(const String &datetime);
    void _displayPage0(const EnvData &data);
    void _displayPage1(const EnvData &data);
    void _displayPage2(const EnvData &data);
    void _displayPage3(const EnvData &data);

public:
    LCDDisplay(uint8_t lcdAddress);
    void setup();
    void clear();
    void showData(const EnvData &data);
    void nextPage();
    void showMessage(const String &line1, const String &line2 = "");
};