#include "../include/SerialTask.hpp"

#include <Arduino.h>

#include "../include/Processor.hpp"

void SerialTask(void* pvParameters) {
    String receivedCommand;

    while (true) {
        if (Serial.available() > 0) {
            char incomingChar = Serial.read();

            if (incomingChar == '$') {
                ProcessCommand(receivedCommand);
                receivedCommand = "";
            } else {
                receivedCommand += incomingChar;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(1));
    }
}
