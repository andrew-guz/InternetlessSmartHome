#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

#include "Display.h"
#include "Memory.h"
#include "Messages.hpp"
#include "Temperature.h"

OLEDDisplay display;
I2CTemperatureSensor thermometer;
Memory memory;

unsigned long lastUpdate = 0;
int brightness = 2;

Mac mac;

void OnDataSent(std::uint8_t* mac_addr, const std::uint8_t sendStatus) {}

void OnDataRecv(std::uint8_t* mac_addr, std::uint8_t* incomingData, const std::uint8_t len) {
    if (len != sizeof(Message))
        return;

    Message* message = (Message*)incomingData;
    if (message->type == MessageType::THERMOMETER_BRIGHTNESS && message->dataSize == sizeof(int)) {
        int newValue;
        memcpy(&newValue, message->data, sizeof(int));

        if (newValue != brightness) {
            brightness = newValue;
            memory.Save("brightness", brightness);

            display.SetBrightness(brightness);
        }
    }
}

void setup() {
    wifi_get_macaddr(STATION_IF, mac.data());

    display.Setup();
    thermometer.Setup();
    memory.Setup();

    brightness = memory.Load("brightness", 2);

    display.SetBrightness(brightness);

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

        float temperature = thermometer.GetTemperature();
        display.SetFont(OLEDDisplay::OLEDDisplay::Font::Font_18pt7b);
        display.ShowString(String(temperature, 1) + String(" C"));

        Message message{
            .type = MessageType::TEMPERATURE,
        };
        memcpy(message.sender.data(), mac.data(), 6);
        memcpy(message.receiver.data(), broadcast, 6);
        memcpy(message.data, &temperature, sizeof(temperature));
        message.dataSize = sizeof(temperature);

        esp_now_send(broadcast, (std::uint8_t*)(&message), sizeof(message));
    }

    delay(1);
}
