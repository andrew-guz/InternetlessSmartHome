#include <Arduino.h>
#include <ArduinoOTA.h>
#include <WString.h>

#include "Defines.h"
#include "DeviceDefines.h"
#include "Display.h"
#include "Memory.h"
#include "Temperature.h"
#include "WiFi.h"

OLEDDisplay display;
I2CTemperatureSensor temperature;
WiFiDataServer server;
Memory memory;

unsigned long lastUpdate = 0;
float temp = 0.0f;
int brightness = 2;

void setup() {
    display.Setup();
    temperature.Setup();
    server.Setup(DEVICE_WIFI_SSID, WIFI_PASSWORD);
    memory.Setup();

    brightness = memory.Load("brightness", 2);

    display.SetBrightness(brightness);

    server.Register("/type", HTTPMethod::HTTP_GET, [&]() {
        return WiFiDataServer::Response{
            .code = 200,
            .contentType = "text/plain",
            .content = DEVICE_TYPE,
        };
    });
    server.Register("/temperature", HTTPMethod::HTTP_GET, [&]() {
        return WiFiDataServer::Response{
            .code = 200,
            .contentType = "text/plain",
            .content = String(temp),
        };
    });
    server.Register("/brightness", HTTPMethod::HTTP_GET, [&]() {
        return WiFiDataServer::Response{
            .code = 200,
            .contentType = "text/plain",
            .content = String(brightness),
        };
    });
    server.Register("/brightness", HTTPMethod::HTTP_POST, [&](const String& body) {
        const int newValue = body.toInt();
        if (newValue != brightness) {
            brightness = newValue;
            memory.Save("brightness", brightness);

            display.SetBrightness(brightness);
        }

        return WiFiDataServer::Response{
            .code = 200,
            .contentType = "text/plain",
            .content = "OK",
        };
    });

    ArduinoOTA.setHostname(DEVICE_OTA_NAME);
    ArduinoOTA.setPasswordHash(OTA_PASSWORD);
    ArduinoOTA.begin();
}

void loop() {
    server.Loop();
    ArduinoOTA.handle();

    if (millis() - lastUpdate > 1000) {
        lastUpdate = millis();

        temp = temperature.GetTemperature();
        display.SetFont(OLEDDisplay::OLEDDisplay::Font::Font_18pt7b);
        display.ShowString(String(temp, 1) + String(" C"));
    }

    delay(1);
}
