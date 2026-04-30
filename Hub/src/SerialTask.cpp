#include "../include/SerialTask.hpp"

#include <Arduino.h>
#include <WString.h>

#include "../include/Processor.hpp"

namespace {

    void SendSerialReply(const String& reply) { Serial.println(reply); }

} // namespace

void SerialTask(void* pvParameters) {
    String receivedCommand;

    while (true) {
        if (Serial.available() > 0) {
            char incomingChar = Serial.read();

            if (incomingChar == '$') {
                ProcessCommand(receivedCommand, SendSerialReply);
                receivedCommand = "";
            } else {
                receivedCommand += incomingChar;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(1));
    }
}
