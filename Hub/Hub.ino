#include <Arduino.h>

#include "include/Data.h"
#include "include/EspNowTask.h"
#include "include/MonitorTask.h"

TaskHandle_t monitorTaskHandle = NULL;
TaskHandle_t espNowTaskHandle = NULL;

void setup() {
    Serial.begin(115200);

    mutex = xSemaphoreCreateMutex();

    LoadData();

    MonitorTaskInit();

    vTaskDelay(pdMS_TO_TICKS(2000));

    xTaskCreate(MonitorTask, "Monitor task", 16384, NULL, 1, &monitorTaskHandle);
    xTaskCreate(EspNowTask, "ESP-NOW task", 16384, NULL, 1, &espNowTaskHandle);
}

void loop() { vTaskDelete(NULL); }
