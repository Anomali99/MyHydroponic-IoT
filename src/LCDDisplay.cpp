#include "LCDDisplay.h"

LCDDisplay::LCDDisplay(uint8_t lcdAddress)
    : _lcd(lcdAddress, 16, 2), _currentPage(0)
{
}

void LCDDisplay::setup()
{
    _lcd.init();
    _lcd.backlight();
    _lcd.clear();
}

void LCDDisplay::clear()
{
    _lcd.clear();
}

void LCDDisplay::showData(const EnvData &data)
{
    _lcd.clear();

    switch (_currentPage)
    {
    case 0:
        _displayPage0(data);
        break;
    case 1:
        _displayPage1(data);
        break;
    case 2:
        _displayPage2(data);
        break;
    case 3:
        _displayPage3(data);
        break;
    default:
        _lcd.setCursor(0, 0);
        _lcd.print("ERROR PAGE!");
        break;
    }
}

void LCDDisplay::nextPage()
{
    _currentPage = (_currentPage + 1) % _maxPages;
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

void LCDDisplay::_displayPage0(const EnvData &data)
{
    _lcd.setCursor(0, 0);
    _lcd.print("pH:");
    _lcd.print(data.ph, 2);
    _lcd.print(" TDS:");
    _lcd.print(data.tds, 0);

    _lcd.setCursor(0, 1);
    _lcd.print("Time:" + _getShortTime(data.datetime));
}

void LCDDisplay::_displayPage1(const EnvData &data)
{
    _lcd.setCursor(0, 0);
    _lcd.print("Temp:");
    _lcd.print(data.temp, 1);
    _lcd.print(" MT:");
    _lcd.print(data.tankMain, 1);

    _lcd.setCursor(0, 1);
    _lcd.print("Time:" + _getShortTime(data.datetime));
}

void LCDDisplay::_displayPage2(const EnvData &data)
{
    _lcd.setCursor(0, 0);
    _lcd.print("Up:");
    _lcd.print(data.tankPhUp, 1);
    _lcd.print(" Dn:");
    _lcd.print(data.tankPhDown, 1);

    _lcd.setCursor(0, 1);
    _lcd.print("Time:" + _getShortTime(data.datetime));
}

void LCDDisplay::_displayPage3(const EnvData &data)
{
    _lcd.setCursor(0, 0);
    _lcd.print("NutA:");
    _lcd.print(data.tankA, 1);
    _lcd.print(" B:");
    _lcd.print(data.tankB, 1);

    _lcd.setCursor(0, 1);
    _lcd.print("Time:" + _getShortTime(data.datetime));
}
