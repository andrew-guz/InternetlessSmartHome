#include "../include/SmsTask.hpp"

#include <Arduino.h>
#include <vector>

#include "../include/Data.hpp"
#include "../include/Mac.hpp"
#include "../include/Utils.hpp"

void ProcessCommand(const String& command) {
    if (command.startsWith("NAME;")) {
        // NAME;00:00:00:00:00:00;newName$
        // NAME;old_name;newName$
        std::vector<String> parts = SplitString(command, ';');
        if (parts.size() != 3) {
            return;
        }

        const String& newName = parts[2];
        if (newName.length() == 0) {
            return;
        }

        if (parts[1].indexOf(":") != -1) {
            Mac mac = MacFromString(parts[1]);
            SetName(mac, newName);
        } else {
            const String oldName = parts[1];
            const std::optional<Mac> mac = FindMacByName(oldName);
            if (mac.has_value()) {
                SetName(mac.value(), newName);
            }
        }
    } else if (command.startsWith("BRIGHTNESS;")) {
        // BRIGHTNESS;00:00:00:00:00:00;value$
        // BRIGHTNESS;name;value$
        std::vector<String> parts = SplitString(command, ';');
        if (parts.size() != 3) {
            return;
        }

        const String& brightnessString = parts[2];
        if (brightnessString.length() == 0) {
            return;
        }

        const int brightness = brightnessString.toInt();

        if (parts[1].indexOf(":") != -1) {
            Mac mac = MacFromString(parts[1]);
            // send brightness message
        } else {
            const String oldName = parts[1];
            const std::optional<Mac> mac = FindMacByName(oldName);
            if (mac.has_value()) {
                // send brightness message
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
