#include "../include/SmsTask.hpp"

#include <Arduino.h>

void SmsTask(void* pvParameters) {
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}
