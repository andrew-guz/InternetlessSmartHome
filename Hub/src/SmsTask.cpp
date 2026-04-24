#include "../include/SmsTask.hpp"

#include <Arduino.h>
#include <esp_now.h>
#include <vector>

#include "../include/Data.hpp"
#include "../include/Mac.hpp"
#include "../include/Utils.hpp"

namespace {
    static std::uint8_t broadcast[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
}

extern Mac myMac;

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

        Mac thermometerMac;
        if (parts[1].indexOf(":") != -1) {
            thermometerMac = MacFromString(parts[1]);
        } else {
            const String oldName = parts[1];
            const std::optional<Mac> macByName = FindMacByName(oldName);
            if (!macByName.has_value()) {
                return;
            }

            thermometerMac = macByName.value();
        }

        // send brightness message
        Message message{
            .type = MessageType::THERMOMETER_BRIGHTNESS,
        };
        memcpy(message.sender.data(), myMac.data(), 6);
        memcpy(message.receiver.data(), thermometerMac.data(), 6);
        setMessageData(message, brightness);
        message.dataSize = sizeof(brightness);

        esp_now_send(broadcast, (std::uint8_t*)(&message), sizeof(message));
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
