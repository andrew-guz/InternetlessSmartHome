#include <Arduino.h>

volatile int counter = 0;
TaskHandle_t task1Handle = NULL;
TaskHandle_t task2Handle = NULL;

void TaskIncrement(void* pvParameters) {
    while (true) {
        counter++;
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void TaskPrint(void* pvParameters) {
    while (true) {
        Serial.print("Current counter value: ");
        Serial.println(counter);

        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("Starting ESP32 Dual-Core RTOS Example...");

    xTaskCreatePinnedToCore(TaskIncrement, "TaskInc", 2048, NULL, 1, &task1Handle, 0);

    // Создание задачи для вывода на Core 1
    xTaskCreate(TaskPrint, "TaskPrint", 2048, NULL, 1, &task2Handle);
}

void loop() { vTaskDelete(NULL); }
