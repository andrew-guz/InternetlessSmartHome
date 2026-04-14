#include <Arduino.h>

#include "include/MonitorTask.h"
#include "include/WiFiTask.h"

TaskHandle_t monitorTaskHandle = NULL;
TaskHandle_t wifiTaskHandle = NULL;

void setup() {
    MonitorTaskInit();

    xTaskCreate(MonitorTask, "Monitor task", 2048, NULL, 1, &monitorTaskHandle);
    xTaskCreate(WiFiTask, "WiFi task", 2048, NULL, 1, &wifiTaskHandle);
}

void loop() { vTaskDelete(NULL); }
