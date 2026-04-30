#include <Arduino.h>
#include <esp_mac.h>
#include <esp_now.h>

#include "./include/Data.hpp"
#include "./include/EspNowTask.hpp"
#include "./include/Memory.hpp"
#include "./include/MonitorTask.hpp"
#include "./include/SerialTask.hpp"
#include "./include/SmsTask.hpp"

Memory memory;

TaskHandle_t monitorTaskHandle = nullptr;
TaskHandle_t espNowTaskHandle = nullptr;
TaskHandle_t smsTaskHandle = nullptr;
TaskHandle_t serialTaskHandle = nullptr;

#define ESP_NOW_MESSAGE_QUEUE_SIZE 100

Mac myMac;

void setup() {
    Serial.begin(115200);

    esp_read_mac(myMac.data(), ESP_MAC_WIFI_STA);

    memory.Setup();

    mutex = xSemaphoreCreateMutex();

    espNowMessagesQueue = xQueueCreate(ESP_NOW_MESSAGE_QUEUE_SIZE, sizeof(Message));

    LoadData();

    MonitorTaskInit();

    EspNowTaskInit();

    vTaskDelay(pdMS_TO_TICKS(2000));

    xTaskCreate(MonitorTask, "Monitor task", 16384, NULL, 1, &monitorTaskHandle);
    xTaskCreate(EspNowTask, "ESP-NOW task", 16384, NULL, 1, &espNowTaskHandle);
    xTaskCreate(SmsTask, "SMS task", 16384, NULL, 1, &smsTaskHandle);
    xTaskCreate(SerialTask, "Serial task", 16384, NULL, 1, &serialTaskHandle);
}

void loop() { vTaskDelete(NULL); }
