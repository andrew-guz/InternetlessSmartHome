#include <Arduino.h>

#include "include/Data.h"
#include "include/MonitorTask.h"
#include "include/WiFiTask.h"

TaskHandle_t monitorTaskHandle = NULL;
TaskHandle_t wifiTaskHandle = NULL;

void setup() {
    Serial.begin(115200);

    mutex = xSemaphoreCreateMutex();

    MonitorTaskInit();

    vTaskDelay(pdMS_TO_TICKS(2000));

    xTaskCreate(MonitorTask, "Monitor task", 16384, NULL, 1, &monitorTaskHandle);
    xTaskCreate(WiFiTask, "WiFi task", 16384, NULL, 1, &wifiTaskHandle);
}

void loop() { vTaskDelete(NULL); }
