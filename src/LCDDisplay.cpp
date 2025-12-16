#include "LCDDisplay.h"

LCDDisplay::LCDDisplay(uint8_t lcdAddress)
    : _lcd(lcdAddress, 16, 2)
{
}

void LCDDisplay::setup()
{
    _lcd.init();
    _lcd.backlight();
    _lcd.clear();
    _scrollPosition = -1;
}

void LCDDisplay::loop()
{
    static unsigned long lastTime = 0;
    unsigned long now = millis();

    if (now - lastTime > 1000 && _scrollPosition != -1)
    {
        lastTime = now;
        _runText();
    }
}

void LCDDisplay::clear()
{
    _lcd.clear();
}

void LCDDisplay::showData(const EnvData &data)
{
    _datetime = "Time:" + _getShortTime(data.datetime);
    _dataMessage = "PH: " + String(data.ph, 1) + ", TDS: " + String(data.tds, 0) + " ppm, Temp: " + String(data.temp, 2) + " C, MT: " + String(data.tankMain, 0) + " ml, Up: " + String(data.tankPhUp, 1) + " ml, Down: " + String(data.tankPhDown, 1) + " ml, A: " + String(data.tankA, 1) + " ml, B: " + String(data.tankB, 1) + " ml";

    _scrollPosition = 0;
    _lcd.clear();
}

void LCDDisplay::showSetupData(const SetupData &setupData)
{
    _lcd.setCursor(0, 0);
    _lcd.print(setupData.name);

    _lcd.setCursor(6, 0);
    _lcd.print("V:" + String(setupData.volume, 1));

    _lcd.setCursor(0, 1);
    _lcd.print("D:" + String(setupData.distance, 1));

    _lcd.setCursor(9, 1);
    _lcd.print("L:" + String(setupData.level, 1));
}

void LCDDisplay::showMessage(const String &line1, const String &line2)
{
    _lcd.clear();
    _lcd.setCursor(0, 0);
    _lcd.print(line1.substring(0, 16));

    if (line2.length() > 0)
    {
        _lcd.setCursor(0, 1);
        _lcd.print(line2.substring(0, 16));
    }
}

String LCDDisplay::_getShortTime(const String &datetime)
{
    if (datetime.length() >= 19)
        return datetime.substring(11, 19);

    if (datetime.length() == 8)
        return datetime;

    return datetime.substring(0, 8);
}

void LCDDisplay::_runText()
{
    const int LCD_WIDTH = 16;
    int messageLength = _dataMessage.length();

    if (messageLength > LCD_WIDTH)
    {
        String displayLine1 = _dataMessage.substring(_scrollPosition);
        displayLine1 = displayLine1.substring(0, LCD_WIDTH);

        _lcd.setCursor(0, 0);
        _lcd.print(displayLine1);

        _scrollPosition++;

        if (_scrollPosition >= (messageLength - LCD_WIDTH + 3))
        {
            _scrollPosition = 0;
        }
    }
    else
    {
        _lcd.setCursor(0, 0);
        _lcd.print(_dataMessage.substring(0, LCD_WIDTH));
    }

    _lcd.setCursor(0, 1);
    _lcd.print(_datetime);
}