#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

// #define TM1637_DISPLAY
#define OLED_DISPLAY

// #define ONE_WIRE_TEMPERATURE_SENSOR
#define I2C_TEMPERATURE_SENSOR

#include "Display.hpp"
#include "Memory.hpp"
#include "Messages.hpp"
#include "Temperature.hpp"

#ifdef TM1637_DISPLAY
TM1637Display display(D3, D4);
#endif // TM1637_DISPLAY

#ifdef OLED_DISPLAY
OLEDDisplay display;
#endif // OLED_DISPLAY

#ifdef ONE_WIRE_TEMPERATURE_SENSOR
OneWireTemperatureSensor thermometer(D3);
#endif // ONE_WIRE_TEMPERATURE_SENSOR

#ifdef I2C_TEMPERATURE_SENSOR
I2CTemperatureSensor thermometer;
#endif // I2C_TEMPERATURE_SENSOR

Memory memory;

unsigned long lastUpdate = 0;
int brightness = 2;

Mac myMac;

void OnDataSent(std::uint8_t* mac_addr, const std::uint8_t sendStatus) {}

void OnDataRecv(std::uint8_t* mac_addr, std::uint8_t* incomingData, const std::uint8_t len) {
    if (len != sizeof(Message))
        return;

    Message* message = (Message*)incomingData;
    if (memcmp(message->receiver.data(), myMac.data(), sizeof(Mac)) == 0 && message->type == MessageType::THERMOMETER_BRIGHTNESS &&
        message->dataSize == sizeof(int)) {
        int newValue = getMessageData<int>(message);
        if (newValue != brightness) {
            brightness = newValue;
            memory.Save("brightness", brightness);

            display.SetBrightness(brightness);
        }
    }
}

void setup() {
    Serial.begin(115200);

    wifi_get_macaddr(STATION_IF, myMac.data());
    Serial.println(MacToString(myMac));

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
#ifdef OLED_DISPLAY
        display.SetFont(OLEDDisplay::OLEDDisplay::Font::Font_18pt7b);
#endif // OLED_DISPLAY
        display.ShowString(String(temperature, 1) + String(" C"));

        Message message{
            .type = MessageType::TEMPERATURE,
        };
        memcpy(message.sender.data(), myMac.data(), 6);
        memcpy(message.receiver.data(), broadcast, 6);
        setMessageData(message, temperature);
        message.dataSize = sizeof(temperature);

        esp_now_send(broadcast, (std::uint8_t*)(&message), sizeof(message));
    }

    delay(1);
}
