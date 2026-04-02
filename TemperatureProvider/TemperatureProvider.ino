#include <Arduino.h>
#include <ArduinoOTA.h>
#include <WString.h>

#include "Defines.h"
#include "Display.h"
#include "Temperature.h"
#include "WiFi.h"

OLEDDisplay display;
I2CTemperatureSensor temperature;
WiFiDataServer server;

float temp = 0.0f;

void setup() {
    display.Setup();
    temperature.Setup();
    server.Setup(DEVICE_WIFI_SSID, DEVICE_WIFI_PASSWORD);
    server.Register("/get", HTTPMethod::HTTP_GET, [&]() {
        return WiFiDataServer::Response{
            .code = 200,
            .contentType = "text/plain",
            .content = String(temp),
        };
    });

    ArduinoOTA.setHostname(DEVICE_OTA_NAME);
    ArduinoOTA.setPasswordHash(DEVICE_OTA_PASSWORD);
    ArduinoOTA.begin();
}

void loop() {
    server.Loop();
    ArduinoOTA.handle();

    temp = temperature.GetTemperature();
    display.SetFont(OLEDDisplay::OLEDDisplay::Font::Font_18pt7b);
    display.ShowString(String(temp, 1) + String(" C"));
}
