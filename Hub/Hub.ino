#include <Arduino.h>

#include "include/Data.h"
#include "include/EspNowTask.h"
#include "include/Messages.hpp"
#include "include/MonitorTask.h"

TaskHandle_t monitorTaskHandle = nullptr;
TaskHandle_t espNowTaskHandle = nullptr;

#define ESP_NOW_MESSAGE_QUEUE_SIZE 100

void setup() {
    Serial.begin(115200);

    mutex = xSemaphoreCreateMutex();

    espNowMessagesQueue = xQueueCreate(ESP_NOW_MESSAGE_QUEUE_SIZE, sizeof(Message));

    LoadData();

    MonitorTaskInit();

    EspNowTaskInit();

    vTaskDelay(pdMS_TO_TICKS(2000));

    xTaskCreate(MonitorTask, "Monitor task", 16384, NULL, 1, &monitorTaskHandle);
    xTaskCreate(EspNowTask, "ESP-NOW task", 16384, NULL, 1, &espNowTaskHandle);
}

void loop() { vTaskDelete(NULL); }
