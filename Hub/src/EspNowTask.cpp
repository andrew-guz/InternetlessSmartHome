#include "../include/EspNowTask.hpp"

#include <Arduino.h>
#include <WiFi.h>
#include <cstdint>
#include <esp_now.h>

#include "../include/Data.hpp"
#include "../include/Messages.hpp"

void OnDataSent(const esp_now_send_info_t* tx_info, esp_now_send_status_t status) {}

void OnDataRecv(const esp_now_recv_info_t* esp_now_info, const uint8_t* data, int data_len) {
    if (data_len != sizeof(Message))
        return;

    Message message;
    memcpy(&message, data, sizeof(message));

    xQueueSendFromISR(espNowMessagesQueue, &message, NULL);
}

void EspNowTaskInit() {
    WiFi.mode(WIFI_STA);

    esp_now_init();

    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);

    static std::uint8_t broadcast[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, broadcast, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    esp_now_add_peer(&peerInfo);
}

void EspNowTask(void* pvParameters) {
    Message message;

    while (true) {
        if (xQueueReceive(espNowMessagesQueue, &message, portMAX_DELAY) == pdTRUE) {
            if (message.type == MessageType::TEMPERATURE) {
                float temperature = getMessageData<float>(message);
                SetThermometerValue(message.sender, temperature);
            } else if (message.type == MessageType::RELAY_STATE) {
                bool state = getMessageData<bool>(message);
                SetRelayState(message.sender, state);
            } else if (message.type == MessageType::THERMOSTAT_RELAY_STATE) {
                RelayState state = getMessageData<RelayState>(message);
                SetThermostatRelayState(message.sender, state);
            }
        }
    }
}
