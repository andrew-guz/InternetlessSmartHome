#include "../include/SmsTask.hpp"

#include <Arduino.h>

#include "../include/Processor.hpp"

void ProcessCommand(const String& command) {
    if (command.startsWith("NAME;")) {
        // NAME;00:00:00:00:00:00;newName$
        // NAME;old_name;newName$
        Rename(command);
    } else if (command.startsWith("BRIGHTNESS;")) {
        // BRIGHTNESS;00:00:00:00:00:00;value$
        // BRIGHTNESS;name;value$
        SetBrightness(command);
    } else if (command.startsWith("STATE;")) {
        // STATE;00:00:00:00:00:00;value$
        // STATE;name;value$
        SetState(command);
    }
}

void SmsTask(void* pvParameters) {
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
