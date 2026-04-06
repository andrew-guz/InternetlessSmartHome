#include <Arduino.h>
#include <ArduinoOTA.h>
#include <WString.h>
#include <optional>

#include "Defines.h"
#include "DeviceDefines.h"
#include "Memory.h"
#include "WiFi.h"

WiFiDataServer server;
Memory memory;

unsigned long lastUpdate = 0;
bool manualMode = false;
bool manualState = false;
String thermometerName = "";
float targetTemperature = 20.0f;
float temperatureDelta = 0.5f;
bool state = false;

void setup() {
    pinMode(RELAY_PIN, OUTPUT);

    server.Setup(DEVICE_WIFI_SSID, WIFI_PASSWORD);
    memory.Setup();

    manualMode = memory.Load("manualMode", false);
    if (manualMode) {
        manualState = memory.Load("manualState", false);
    }

    thermometerName = memory.Load("thermometerName", String{});
    targetTemperature = memory.Load("targetTemperature", 20.0f);
    temperatureDelta = memory.Load("temperatureDelta", 0.5f);

    if (manualMode) {
        state = manualState;
        digitalWrite(RELAY_PIN, manualState ? (ON_BY_HIGH_LEVEL ? HIGH : LOW) : (ON_BY_HIGH_LEVEL ? LOW : HIGH));
    }

    server.Register("/state", HTTPMethod::HTTP_GET, [&]() {
        return WiFiDataServer::Response{
            .code = 200,
            .contentType = "text/plain",
            .content = state ? "true" : "false",
        };
    });
    server.Register("/temperature", HTTPMethod::HTTP_POST, [&](const String& body) {
        if (manualMode) {
            return WiFiDataServer::Response{
                .code = 300,
                .contentType = "text/plain",
                .content = "In manual mode",
            };
        }

        const float temperature = body.toFloat();
        if (temperature < targetTemperature - temperatureDelta) {
            state = true;
            digitalWrite(RELAY_PIN, ON_BY_HIGH_LEVEL ? HIGH : LOW);
        } else if (temperature > targetTemperature + temperatureDelta) {
            state = false;
            digitalWrite(RELAY_PIN, ON_BY_HIGH_LEVEL ? LOW : HIGH);
        }

        return WiFiDataServer::Response{
            .code = 200,
            .contentType = "text/plain",
            .content = "OK",
        };
    });
    server.Register("/manualMode", HTTPMethod::HTTP_GET, [&]() {
        return WiFiDataServer::Response{
            .code = 200,
            .contentType = "text/plain",
            .content = manualMode ? "true" : "false",
        };
    });
    server.Register("/manualMode", HTTPMethod::HTTP_POST, [&](const String& body) {
        const bool newValue = body == "true";
        if (newValue != manualMode) {
            manualMode = newValue;
            memory.Save("manualMode", manualMode);
        }

        return WiFiDataServer::Response{
            .code = 200,
            .contentType = "text/plain",
            .content = "OK",
        };
    });
    server.Register("/manualState", HTTPMethod::HTTP_GET, [&]() {
        return WiFiDataServer::Response{
            .code = 200,
            .contentType = "text/plain",
            .content = manualState ? "true" : "false",
        };
    });
    server.Register("/manualState", HTTPMethod::HTTP_POST, [&](const String& body) {
        if (manualMode == false) {
            return WiFiDataServer::Response{
                .code = 300,
                .contentType = "text/plain",
                .content = "Not in manual mode",
            };
        }

        const bool newValue = body == "true";
        if (newValue != manualState) {
            manualState = newValue;
            memory.Save("manualState", manualState);

            state = manualState;
            digitalWrite(RELAY_PIN, manualState ? (ON_BY_HIGH_LEVEL ? HIGH : LOW) : (ON_BY_HIGH_LEVEL ? LOW : HIGH));
        }

        return WiFiDataServer::Response{
            .code = 200,
            .contentType = "text/plain",
            .content = "OK",
        };
    });
    server.Register("/thermometerName", HTTPMethod::HTTP_GET, [&]() {
        return WiFiDataServer::Response{
            .code = 200,
            .contentType = "text/plain",
            .content = thermometerName,
        };
    });
    server.Register("/thermometerName", HTTPMethod::HTTP_POST, [&](const String& body) {
        if (body != thermometerName) {
            thermometerName = body;
            memory.Save("thermometerName", thermometerName);
        }

        return WiFiDataServer::Response{
            .code = 200,
            .contentType = "text/plain",
            .content = "OK",
        };
    });
    server.Register("/targetTemperature", HTTPMethod::HTTP_GET, [&]() {
        return WiFiDataServer::Response{
            .code = 200,
            .contentType = "text/plain",
            .content = String(targetTemperature),
        };
    });
    server.Register("/targetTemperature", HTTPMethod::HTTP_POST, [&](const String& body) {
        const float newValue = body.toFloat();
        if (newValue != targetTemperature) {
            targetTemperature = body.toFloat();
            memory.Save("targetTemperature", targetTemperature);
        }

        return WiFiDataServer::Response{
            .code = 200,
            .contentType = "text/plain",
            .content = "OK",
        };
    });
    server.Register("/temperatureDelta", HTTPMethod::HTTP_GET, [&]() {
        return WiFiDataServer::Response{
            .code = 200,
            .contentType = "text/plain",
            .content = String(temperatureDelta),
        };
    });
    server.Register("/temperatureDelta", HTTPMethod::HTTP_POST, [&](const String& body) {
        const float newValue = body.toFloat();
        if (newValue != temperatureDelta) {
            temperatureDelta = body.toFloat();
            memory.Save("temperatureDelta", temperatureDelta);
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

    delay(1);
}
