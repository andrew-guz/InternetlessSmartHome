#ifndef _TEMPERATURE_H_
#define _TEMPERATURE_H_

#include <Adafruit_BMP085.h>

class I2CTemperatureSensor {
public:
    I2CTemperatureSensor() = default;

    void Setup() { _sensor.begin(); }

    float GetTemperature() { return _sensor.readTemperature(); }

private:
    Adafruit_BMP085 _sensor;
};

#endif // _TEMPERATURE_H_
