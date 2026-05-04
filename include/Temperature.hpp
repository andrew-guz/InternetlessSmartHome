#ifndef _TEMPERATURE_H_
#define _TEMPERATURE_H_

#ifdef ONE_WIRE_TEMPERATURE_SENSOR
#include <DallasTemperature.h>
#include <OneWire.h>

class OneWireTemperatureSensor {
public:
    OneWireTemperatureSensor(int pin) :
        _oneWire(pin),
        _sensor(&_oneWire) {}

    void Setup() {
        _sensor.begin();
        _sensor.setResolution(12);
    }

    float GetTemperature() {
        _sensor.requestTemperatures();
        return _sensor.getTempCByIndex(0);
    }

private:
    OneWire _oneWire;
    DallasTemperature _sensor;
};
#endif // ONE_WIRE_TEMPERATURE_SENSOR

#ifdef I2C_TEMPERATURE_SENSOR
#include <Adafruit_BMP085.h>

class I2CTemperatureSensor {
public:
    I2CTemperatureSensor() = default;

    void Setup() { _sensor.begin(); }

    float GetTemperature() { return _sensor.readTemperature(); }

private:
    Adafruit_BMP085 _sensor;
};
#endif // I2C_TEMPERATURE_SENSOR

#endif // _TEMPERATURE_H_
