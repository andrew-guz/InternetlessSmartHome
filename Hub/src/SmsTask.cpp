#include "../include/SmsTask.hpp"

#include <Arduino.h>
#include <vector>

#include "../include/Data.hpp"
#include "../include/Mac.hpp"
#include "../include/Utils.hpp"

void ProcessCommand(const String& command) {
    if (command.startsWith("NAME;")) {
        // NAME;00:00:00:00:00:00;name$
        std::vector<String> parts = SplitString(command, ';');
        if (parts.size() == 3) {
            Mac mac = MacFromString(parts[1]);
            const String& name = parts[2];
            if (name.length()) {
                SetName(mac, name);
            }
        }
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
