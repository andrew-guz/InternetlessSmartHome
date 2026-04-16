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

    xTaskCreate(MonitorTask, "Monitor task", 2048, NULL, 1, &monitorTaskHandle);
    xTaskCreate(WiFiTask, "WiFi task", 2048, NULL, 1, &wifiTaskHandle);

    vTaskDelay(pdMS_TO_TICKS(500));

    ScanDevices();
}

void loop() { vTaskDelete(NULL); }
