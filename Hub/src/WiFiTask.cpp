#include "WiFiTask.h"

#include <Arduino.h>

void WiFiTask(void* pvParameters) {
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}
