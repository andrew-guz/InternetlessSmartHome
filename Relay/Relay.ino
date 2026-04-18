#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <cstring>
#include <espnow.h>

#include "Memory.h"
#include "Messages.hpp"

Memory memory;

#define RELAY_PIN        LED_BUILTIN
#define ON_BY_HIGH_LEVEL false

unsigned long lastUpdate = 0;
bool state;

Mac mac;

void OnDataSent(std::uint8_t* mac_addr, const std::uint8_t sendStatus) {}

void OnDataRecv(std::uint8_t* mac_addr, std::uint8_t* incomingData, const std::uint8_t len) {
    if (len != sizeof(Message))
        return;

    Message* message = (Message*)incomingData;
    if (memcmp(message->receiver.data(), mac.data(), sizeof(Mac)) == 0 && message->type == MessageType::RELAY_SET_STATE &&
        message->dataSize == sizeof(bool)) {
        bool newState;
        memcpy(&newState, message->data, sizeof(newState));
        if (newState != state) {
            state = newState;
            digitalWrite(RELAY_PIN, state ? (ON_BY_HIGH_LEVEL ? HIGH : LOW) : (ON_BY_HIGH_LEVEL ? LOW : HIGH));
            memory.Save("state", state);
        }
    }
}

void setup() {
    Serial.begin(115200);

    wifi_get_macaddr(STATION_IF, mac.data());

    memory.Setup();

    state = memory.Load("state", false);

    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, state ? (ON_BY_HIGH_LEVEL ? HIGH : LOW) : (ON_BY_HIGH_LEVEL ? LOW : HIGH));

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
            .type = MessageType::RELAY_STATE,
        };
        memcpy(message.sender.data(), mac.data(), 6);
        memcpy(message.receiver.data(), broadcast, 6);
        memcpy(message.data, &state, sizeof(bool));
        message.dataSize = sizeof(bool);

        esp_now_send(broadcast, (std::uint8_t*)(&message), sizeof(message));
    }

    delay(1);
}
