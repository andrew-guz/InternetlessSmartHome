#include <Arduino.h>
#include <ArduinoOTA.h>
#include <WString.h>

#include "Defines.h"
#include "DeviceDefines.h"
#include "Memory.h"
#include "WiFi.h"

WiFiDataServer server;
Memory memory;

bool on = false;

void setup() {
    pinMode(RELAY_PIN, OUTPUT);

    server.Setup(DEVICE_WIFI_SSID, WIFI_PASSWORD);
    memory.Setup();

    on = memory.Load("on", false);
    digitalWrite(RELAY_PIN, on ? (ON_BY_HIGH_LEVEL ? HIGH : LOW) : (ON_BY_HIGH_LEVEL ? LOW : HIGH));

    server.Register("/state", HTTPMethod::HTTP_GET, [&]() {
        return WiFiDataServer::Response{
            .code = 200,
            .contentType = "text/plain",
            .content = on ? "true" : "false",
        };
    });
    server.Register("/state", HTTPMethod::HTTP_POST, [&](const String& body) {
        on = body == "true";
        memory.Save("on", on);

        digitalWrite(RELAY_PIN, on ? (ON_BY_HIGH_LEVEL ? HIGH : LOW) : (ON_BY_HIGH_LEVEL ? LOW : HIGH));

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
