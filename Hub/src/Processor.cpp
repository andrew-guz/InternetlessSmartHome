#include "../include/Processor.hpp"

#include <WString.h>
#include <esp_now.h>
#include <optional>
#include <vector>

#include "../include/Data.hpp"
#include "../include/Mac.hpp"
#include "../include/Messages.hpp"
#include "../include/Utils.hpp"
#include "include/Messages.hpp"

extern Mac myMac;

namespace {
    static std::uint8_t broadcast[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

    std::optional<Mac> GetReceiverMac(const String& macString) {
        std::optional<Mac> receiverMac = std::nullopt;
        if (macString.indexOf(":") != -1) {
            receiverMac = MacFromString(macString);
        } else {
            receiverMac = FindMacByName(macString);
        }

        return receiverMac;
    }

    template<typename T>
    void SendMessage(const MessageType type, const Mac& receiverMac, const T& data) {
        Message message{
            .type = type,
        };
        memcpy(message.sender.data(), myMac.data(), 6);
        memcpy(message.receiver.data(), receiverMac.data(), 6);
        setMessageData(message, data);
        message.dataSize = sizeof(data);

        esp_now_send(broadcast, (std::uint8_t*)(&message), sizeof(message));
    }
} // namespace

void ProcessCommand(const String& command) {
    std::vector<String> commands = SplitString(command, '$');
    for (const String& oneCommand : commands) {
        if (oneCommand.startsWith("SET_NAME;")) {
            // SET_NAME;00:00:00:00:00:00;newName$
            // SET_NAME;old_name;newName$
            Rename(oneCommand);
        } else if (oneCommand.startsWith("SET_BRIGHTNESS;")) {
            // SET_BRIGHTNESS;00:00:00:00:00:00;value$
            // SET_BRIGHTNESS;name;value$
            SetBrightness(oneCommand);
        } else if (oneCommand.startsWith("SET_STATE;")) {
            // SET_STATE;00:00:00:00:00:00;value$
            // SET_STATE;name;value$
            SetState(oneCommand);
        }
    }
}

// SET_NAME;00:00:00:00:00:00;newName$
// SET_NAME;old_name;newName$
void Rename(const String& command) {
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
}

// SET_BRIGHTNESS;00:00:00:00:00:00;value$
// SET_BRIGHTNESS;name;value$
void SetBrightness(const String& command) {
    std::vector<String> parts = SplitString(command, ';');
    if (parts.size() != 3) {
        return;
    }

    const String& brightnessString = parts[2];
    if (brightnessString.length() == 0) {
        return;
    }

    const int brightness = brightnessString.toInt();

    std::optional<Mac> receiverMac = GetReceiverMac(parts[1]);
    if (!receiverMac.has_value()) {
        return;
    }

    SendMessage(MessageType::THERMOMETER_BRIGHTNESS, receiverMac.value(), brightness);
}

// SET_STATE;00:00:00:00:00:00;value$
// SET_STATE;name;value$
void SetState(const String& command) {
    std::vector<String> parts = SplitString(command, ';');
    if (parts.size() != 3) {
        return;
    }

    const String& stateString = parts[2];
    if (stateString.length() == 0) {
        return;
    }

    const bool state = (stateString == "true" || stateString == "1" ? true : false);

    std::optional<Mac> receiverMac = GetReceiverMac(parts[1]);
    if (!receiverMac.has_value()) {
        return;
    }

    // SendMessage(MessageType::RELAY_SET_STATE, receiverMac.value(), state);
}
