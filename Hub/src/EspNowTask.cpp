#include "../include/EspNowTask.h"

#include <Arduino.h>

void EspNowTask(void* pvParameters) {
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
