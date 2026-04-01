#pragma once

#include <Adafruit_BMP085.h>

class I2CTemperatureSensor {
public:
    I2CTemperatureSensor() = default;

    void Setup() { _sensor.begin(); }

    float GetTemperature() { return _sensor.readTemperature(); }

private:
    Adafruit_BMP085 _sensor;
};
