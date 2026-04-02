#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <Wire.h>

#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans18pt7b.h>

#define DISPLAY_MAX_BRIGHTNESS 1
#define SCREEN_WIDTH           128
#define SCREEN_HEIGHT          32
#define OLED_RESET             -1
#define SCREEN_ADDRESS         0x3C

class OLEDDisplay {
public:
    OLEDDisplay() :
        _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET) {}

    void Setup() {
        _display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
        _display.setTextSize(1);
        _display.setTextColor(SSD1306_WHITE);
        _display.cp437(true);
    }

    enum class Font {
        Font_12pt7b,
        Font_18pt7b,
    };

    void SetFont(Font font) {
        switch (font) {
            case Font::Font_12pt7b:
                _display.setFont(&FreeSans12pt7b);
                break;
            case Font::Font_18pt7b:
                _display.setFont(&FreeSans18pt7b);
                break;
        }
    }

    void SetBrightness(int value) {
        if (value < 0)
            value = 0;
        if (value > 1)
            value = 1;
        _brightness = value;
        if (_brightness == 0) {
            _display.clearDisplay();
        }
    }

    void ShowString(const String& str) {
        _display.clearDisplay();
        if (_brightness == 0) {
            _display.display();
            return;
        }

        int16_t x1, y1;
        uint16_t w, h;
        _display.getTextBounds(str, 0, 0, &x1, &y1, &w, &h);
        _display.setCursor((SCREEN_WIDTH - w) / 2, (SCREEN_HEIGHT - h) / 2 - y1);
        _display.println(str);
        _display.display();
    }

private:
    Adafruit_SSD1306 _display;
    int _brightness = 1;
};
