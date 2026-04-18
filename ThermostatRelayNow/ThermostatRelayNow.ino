#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <cstring>
#include <espnow.h>

#include "Memory.h"
#include "Messages.hpp"

struct RelayState {
    bool manualMode;
    bool manualState;
    Mac thermometer;
    float targetTemperature;
    float temperatureDelta;
    bool temperatureState;
};

Memory memory;

#define RELAY_PIN        LED_BUILTIN
#define ON_BY_HIGH_LEVEL false

unsigned long lastUpdate = 0;
RelayState relayState;

Mac mac;

void OnDataSent(std::uint8_t* mac_addr, const std::uint8_t sendStatus) {}

void OnDataRecv(std::uint8_t* mac_addr, std::uint8_t* incomingData, const std::uint8_t len) {
    if (len != sizeof(Message))
        return;

    Message* message = (Message*)incomingData;
    if (message->type == MessageType::TEMPERATURE && memcmp(message->sender.data(), relayState.thermometer.data(), sizeof(Mac)) == 0 &&
        message->dataSize == sizeof(float)) //
    {
        float temperature;
        memcpy(&temperature, message->data, sizeof(temperature));

        if (relayState.manualMode == false) {
            if (temperature < relayState.targetTemperature - relayState.temperatureDelta && relayState.temperatureState != true) {
                relayState.temperatureState = true;
                digitalWrite(RELAY_PIN, ON_BY_HIGH_LEVEL ? HIGH : LOW);
                memory.Save("temperatureState", relayState.temperatureState);
            } else if (temperature > relayState.targetTemperature + relayState.temperatureDelta && relayState.temperatureState != false) {
                relayState.temperatureState = false;
                digitalWrite(RELAY_PIN, ON_BY_HIGH_LEVEL ? LOW : HIGH);
                memory.Save("temperatureState", relayState.temperatureState);
            }
        }
    } else if (memcmp(message->receiver.data(), mac.data(), sizeof(Mac)) == 0) {
        if (message->type == MessageType::THERMOSTAT_RELAY_MANUAL_MODE && message->dataSize == sizeof(bool)) {
            bool manualMode;
            memcpy(&manualMode, message->data, sizeof(manualMode));
            if (manualMode != relayState.manualMode) {
                relayState.manualMode = manualMode;
                digitalWrite(RELAY_PIN, relayState.manualState ? (ON_BY_HIGH_LEVEL ? HIGH : LOW) : (ON_BY_HIGH_LEVEL ? LOW : HIGH));
                memory.Save("manualMode", relayState.manualMode);
            }
        } else if (message->type == MessageType::THERMOSTAT_RELAY_MANUAL_STATE && message->dataSize == sizeof(bool) &&
                   relayState.manualMode == true) {
            bool manualState;
            memcpy(&manualState, message->data, sizeof(manualState));
            if (relayState.manualMode == true && manualState != relayState.manualState) {
                relayState.manualState = manualState;
                digitalWrite(RELAY_PIN, relayState.manualState ? (ON_BY_HIGH_LEVEL ? HIGH : LOW) : (ON_BY_HIGH_LEVEL ? LOW : HIGH));
                memory.Save("manualState", relayState.manualState);
            }
        } else if (message->type == MessageType::THERMOSTAT_RELAY_THERMOMETER && message->dataSize == sizeof(Mac)) {
            Mac thermometer;
            memcpy(thermometer.data(), message->data, sizeof(Mac));
            if (memcmp(thermometer.data(), relayState.thermometer.data(), sizeof(Mac)) != 0) {
                memcpy(relayState.thermometer.data(), thermometer.data(), sizeof(Mac));
                memory.Save("thermometer", relayState.thermometer);
            }
        } else if (message->type == MessageType::THERMOSTAT_RELAY_TEMPERATURE && message->dataSize == sizeof(float)) {
            float targetTemperature;
            memcpy(&targetTemperature, message->data, sizeof(relayState.targetTemperature));
            if (targetTemperature != relayState.targetTemperature) {
                relayState.targetTemperature = targetTemperature;
                memory.Save("targetTemperature", relayState.targetTemperature);
            }
        } else if (message->type == MessageType::THERMOSTAT_RELAY_TEMPERATURE_DELTA && message->dataSize == sizeof(float)) {
            float temperatureDelta;
            memcpy(&temperatureDelta, message->data, sizeof(relayState.targetTemperature));
            if (temperatureDelta != relayState.temperatureDelta) {
                relayState.temperatureDelta = temperatureDelta;
                memory.Save("temperatureDelta", relayState.temperatureDelta);
            }
        }
    }
}

void setup() {
    Serial.begin(115200);

    wifi_get_macaddr(STATION_IF, mac.data());

    memory.Setup();

    relayState.manualMode = memory.Load("manualMode", false);
    relayState.manualState = memory.Load("manualState", false);
    // relayState.thermometer = memory.Load("thermometer", Mac{}); // TODO while no HUB to configure
    relayState.thermometer = { 0x48, 0x55, 0x19, 0x7B, 0x3B, 0xFD };
    relayState.targetTemperature = memory.Load("targetTemperature", 20.0f);
    relayState.temperatureDelta = memory.Load("temperatureDelta", 0.5f);
    relayState.temperatureState = memory.Load("temperatureState", false);

    pinMode(RELAY_PIN, OUTPUT);
    if (relayState.manualMode == true) {
        digitalWrite(RELAY_PIN, relayState.manualState ? (ON_BY_HIGH_LEVEL ? HIGH : LOW) : (ON_BY_HIGH_LEVEL ? LOW : HIGH));
    } else {
        digitalWrite(RELAY_PIN, relayState.temperatureState ? (ON_BY_HIGH_LEVEL ? HIGH : LOW) : (ON_BY_HIGH_LEVEL ? LOW : HIGH));
    }

    WiFi.mode(WIFI_STA);

    esp_now_init();

    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
    static std::uint8_t broadcast[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

    if (millis() - lastUpdate > 1000) {
        lastUpdate = millis();

        Message message{
            .type = MessageType::THERMOSTAT_RELAY_STATE,
        };
        memcpy(message.sender.data(), mac.data(), 6);
        memcpy(message.data, &relayState, sizeof(RelayState));
        memcpy(message.receiver.data(), broadcast, 6);
        message.dataSize = sizeof(RelayState);

        esp_now_send(broadcast, (std::uint8_t*)(&message), sizeof(message));
    }

    delay(1);
}
