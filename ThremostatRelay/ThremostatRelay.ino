#include <Arduino.h>
#include <ArduinoOTA.h>
#include <WString.h>

#include "Defines.h"
#include "DeviceDefines.h"
#include "Memory.h"
#include "WiFi.h"

WiFiDataServer server;
Memory memory;

bool manualMode = false;
String temperatureName = "";
float targetTemperature = 20.0f;
bool on = false;

void setup() {
    pinMode(RELAY_PIN, OUTPUT);

    server.Setup(DEVICE_WIFI_SSID, WIFI_PASSWORD);
    memory.Setup();

    manualMode = memory.Load("manualMode", false);
    temperatureName = memory.Load("temperatureName", String{});
    targetTemperature = memory.Load("targetTemperature", 20.0f);
    on = memory.Load("on", false);

    digitalWrite(RELAY_PIN, on ? (ON_BY_HIGH_LEVEL ? HIGH : LOW) : (ON_BY_HIGH_LEVEL ? LOW : HIGH));

    server.Register("/manualMode", HTTPMethod::HTTP_GET, [&]() {
        return WiFiDataServer::Response{
            .code = 200,
            .contentType = "text/plain",
            .content = manualMode ? "true" : "false",
        };
    });
    server.Register("/manualMode", HTTPMethod::HTTP_POST, [&](const String& body) {
        manualMode = body == "true";
        memory.Save("manualMode", manualMode);

        return WiFiDataServer::Response{
            .code = 200,
            .contentType = "text/plain",
            .content = "OK",
        };
    });
    server.Register("/temperatureName", HTTPMethod::HTTP_GET, [&]() {
        return WiFiDataServer::Response{
            .code = 200,
            .contentType = "text/plain",
            .content = temperatureName,
        };
    });
    server.Register("/temperatureName", HTTPMethod::HTTP_POST, [&](const String& body) {
        temperatureName = body;
        memory.Save("temperatureName", temperatureName);

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
        targetTemperature = body.toFloat();
        memory.Save("targetTemperature", targetTemperature);

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
