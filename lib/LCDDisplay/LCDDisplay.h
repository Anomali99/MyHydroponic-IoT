#pragma once
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

struct DisplayData
{
    float mainTankLevel;
    float phUpLevel;
    float phDownLevel;
    float nutrientALevel;
    float nutrientBLevel;
    float ph;
    float tds;
    String datetime;
};

class LCDDisplay
{
private:
    LiquidCrystal_I2C _lcd;
    uint8_t _currentPage;
    const uint8_t _maxPages = 4;
    String _getShortTime(const String &datetime);
    void _displayPage0(const DisplayData &data);
    void _displayPage1(const DisplayData &data);
    void _displayPage2(const DisplayData &data);
    void _displayPage3(const DisplayData &data);

public:
    LCDDisplay(uint8_t lcdAddress);
    void setup();
    void clear();
    void showData(const DisplayData &data);
    void nextPage();
    void showMessage(const String &line1, const String &line2 = "");
};