#include "../include/WiFiTask.h"

#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <vector>

#include "../include/Data.h"
#include "../include/Defines.h"

namespace {
#define WIFI_DELAY 200

    void Disconnect() {
        WiFi.disconnect(true);

        vTaskDelay(pdMS_TO_TICKS(WIFI_DELAY));

        WiFi.mode(WIFI_OFF);

        vTaskDelay(pdMS_TO_TICKS(WIFI_DELAY));
    }

} // namespace

void ScanDevices() {
    Serial.println("Scaning devices...");

    ClearData();

    WiFi.mode(WIFI_STA);
    WiFi.disconnect(true);

    vTaskDelay(pdMS_TO_TICKS(WIFI_DELAY));

    const int16_t networksFoundCount = WiFi.scanNetworks();

    vTaskDelay(pdMS_TO_TICKS(WIFI_DELAY));

    Serial.println("Found " + String(networksFoundCount) + " networks");

    if (networksFoundCount == 0) {
        return;
    }

    std::vector<String> ssids;

    for (int16_t i = 0; i < networksFoundCount; ++i) {
        String ssid = WiFi.SSID(i);

        if (ssid.startsWith(WIFI_NETWORK_PREFIX)) {
            Serial.println(ssid);

            ssids.push_back(ssid);
        }
    }

    WiFi.scanDelete();

    vTaskDelay(pdMS_TO_TICKS(WIFI_DELAY));

    for (const String& ssid : ssids) {
        Serial.println("Connecting to " + ssid);

        Disconnect();

        WiFi.mode(WIFI_STA);
        vTaskDelay(pdMS_TO_TICKS(WIFI_DELAY));

        WiFi.begin(ssid.c_str(), WIFI_PASSWORD);

        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 5000 / WIFI_DELAY) {
            vTaskDelay(pdMS_TO_TICKS(WIFI_DELAY));
            attempts++;
        }

        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("Failed to connect to " + ssid);
            continue;
        }

        Serial.println("Connected to " + ssid);

        HTTPClient http;
        http.setTimeout(2000);
        http.begin("http://192.168.4.1/type");
        int httpCode = http.GET();
        if (httpCode == 200) {
            String type = http.getString();
            if (type == DEVICE_TYPE_RELAY) {
                Serial.println("Relay found");
                AddRelay(ssid);
            } else if (type == DEVICE_TYPE_THERMOMETER) {
                Serial.println("Thermometer found");
                AddThermometer(ssid);
            } else if (type == DEVICE_TYPE_THERMOSTAT_RELAY) {
                Serial.println("Thermostat relay found");
                AddThermostatRelay(ssid);
            }
        } else {
            Serial.println("Failed to get type");
        }
        http.end();

        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);

        vTaskDelay(pdMS_TO_TICKS(WIFI_DELAY));
    }

    Serial.println("Scaning devices end");
}

void WiFiTask(void* pvParameters) {
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}
