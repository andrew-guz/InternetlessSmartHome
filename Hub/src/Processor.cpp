#include "../include/Processor.hpp"

#include <WString.h>
#include <esp_now.h>
#include <optional>
#include <set>
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
            // SET_BRIGHTNESS;ALL;value$
            SetBrightness(oneCommand);
        } else if (oneCommand.startsWith("SET_STATE;")) {
            // SET_STATE;00:00:00:00:00:00;value$
            // SET_STATE;name;value$
            // SET_STATE;ALL;value$
            SetState(oneCommand);
        } else if (oneCommand.startsWith("SET_MANUAL_MODE;")) {
            // SET_MANUAL_MODE;00:00:00:00:00:00;value$
            // SET_MANUAL_MODE;name;value$
            // SET_MANUAL_MODE;ALL;value$
            SetManualMode(oneCommand);
        } else if (oneCommand.startsWith("SET_MANUAL_STATE;")) {
            // SET_MANUAL_STATE;00:00:00:00:00:00;value$
            // SET_MANUAL_STATE;name;value$
            // SET_MANUAL_STATE;ALL;value$
            SetManualState(oneCommand);
        } else if (oneCommand.startsWith("SET_RELAY_THERMOMETER;")) {
            // SET_RELAY_THERMOMETER;00:00:00:00:00:00;00:00:00:00:00:00$
            // SET_RELAY_THERMOMETER;name;00:00:00:00:00:00$
            SetRelayThermometer(oneCommand);
        } else if (oneCommand.startsWith("SET_RELAY_TEMPERATURE;")) {
            // SET_RELAY_TEMPERATURE;00:00:00:00:00:00;value$
            // SET_RELAY_TEMPERATURE;name;value$
            // SET_RELAY_TEMPERATURE;ALL;value$
            SetRelayTemperature(oneCommand);
        } else if (oneCommand.startsWith("SET_RELAY_TEMPERATURE_DELTAa;")) {
            // SET_RELAY_TEMPERATURE_DELTA;00:00:00:00:00:00;value$
            // SET_RELAY_TEMPERATURE_DELTA;name;value$
            // SET_RELAY_TEMPERATURE_DELTA;ALL;value$
            SetRelayTemperatureDelta(oneCommand);
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
// SET_BRIGHTNESS;ALL;value$
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

    if (parts[1] == "ALL") {
        std::set<Mac> thermometers = ListThermometers();
        for (const Mac& thermometer : thermometers) {
            SendMessage(MessageType::THERMOMETER_BRIGHTNESS, thermometer, brightness);
        }

        return;
    }

    std::optional<Mac> receiverMac = GetReceiverMac(parts[1]);
    if (!receiverMac.has_value()) {
        return;
    }

    SendMessage(MessageType::THERMOMETER_BRIGHTNESS, receiverMac.value(), brightness);
}

// SET_STATE;00:00:00:00:00:00;value$
// SET_STATE;name;value$
// SET_STATE;ALL;value$
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

    if (parts[1] == "ALL") {
        std::set<Mac> relays = ListRelays();
        for (const Mac& relay : relays) {
            SendMessage(MessageType::RELAY_SET_STATE, relay, state);
        }

        return;
    }

    std::optional<Mac> receiverMac = GetReceiverMac(parts[1]);
    if (!receiverMac.has_value()) {
        return;
    }

    SendMessage(MessageType::RELAY_SET_STATE, receiverMac.value(), state);
}

// SET_MANUAL_MODE;00:00:00:00:00:00;value$
// SET_MANUAL_MODE;name;value$
// SET_MANUAL_MODE;ALL;value$
void SetManualMode(const String& command) {
    std::vector<String> parts = SplitString(command, ';');
    if (parts.size() != 3) {
        return;
    }

    const String& manualModeString = parts[2];
    if (manualModeString.length() == 0) {
        return;
    }

    const bool manualMode = (manualModeString == "true" || manualModeString == "1" ? true : false);

    if (parts[1] == "ALL") {
        std::set<Mac> thermostatRelays = ListThermostatRelays();
        for (const Mac& thermostatRelay : thermostatRelays) {
            SendMessage(MessageType::THERMOSTAT_RELAY_MANUAL_MODE, thermostatRelay, manualMode);
        }

        return;
    }

    std::optional<Mac> receiverMac = GetReceiverMac(parts[1]);
    if (!receiverMac.has_value()) {
        return;
    }

    SendMessage(MessageType::THERMOSTAT_RELAY_MANUAL_MODE, receiverMac.value(), manualMode);
}

// SET_MANUAL_STATE;00:00:00:00:00:00;value$
// SET_MANUAL_STATE;name;value$
// SET_MANUAL_STATE;ALL;value$
void SetManualState(const String& command) {
    std::vector<String> parts = SplitString(command, ';');
    if (parts.size() != 3) {
        return;
    }

    const String& manualStateString = parts[2];
    if (manualStateString.length() == 0) {
        return;
    }

    const bool manualState = (manualStateString == "true" || manualStateString == "1" ? true : false);

    if (parts[1] == "ALL") {
        std::set<Mac> thermostatRelays = ListThermostatRelays();
        for (const Mac& thermostatRelay : thermostatRelays) {
            SendMessage(MessageType::THERMOSTAT_RELAY_MANUAL_STATE, thermostatRelay, manualState);
        }

        return;
    }

    std::optional<Mac> receiverMac = GetReceiverMac(parts[1]);
    if (!receiverMac.has_value()) {
        return;
    }

    SendMessage(MessageType::THERMOSTAT_RELAY_MANUAL_STATE, receiverMac.value(), manualState);
}

// SET_RELAY_THERMOMETER;00:00:00:00:00:00;00:00:00:00:00:00$
// SET_RELAY_THERMOMETER;name;00:00:00:00:00:00$
void SetRelayThermometer(const String& command) {
    std::vector<String> parts = SplitString(command, ';');
    if (parts.size() != 3) {
        return;
    }

    std::optional<Mac> receiverMac = GetReceiverMac(parts[1]);
    if (!receiverMac.has_value()) {
        return;
    }

    std::optional<Mac> thermometerMac = GetReceiverMac(parts[2]);
    if (!thermometerMac.has_value()) {
        return;
    }

    SendMessage(MessageType::THERMOSTAT_RELAY_THERMOMETER, receiverMac.value(), thermometerMac.value());
}

// SET_RELAY_TEMPERATURE;00:00:00:00:00:00;value$
// SET_RELAY_TEMPERATURE;name;value$
// SET_RELAY_TEMPERATURE;ALL;value$
void SetRelayTemperature(const String& command) {
    std::vector<String> parts = SplitString(command, ';');
    if (parts.size() != 3) {
        return;
    }

    const String& relyTemperatureString = parts[2];
    if (relyTemperatureString.length() == 0) {
        return;
    }

    const float relayTemperature = relyTemperatureString.toFloat();

    if (parts[1] == "ALL") {
        std::set<Mac> thermostatRelays = ListThermostatRelays();
        for (const Mac& thermostatRelay : thermostatRelays) {
            SendMessage(MessageType::THERMOSTAT_RELAY_TEMPERATURE, thermostatRelay, relayTemperature);
        }

        return;
    }

    std::optional<Mac> receiverMac = GetReceiverMac(parts[1]);
    if (!receiverMac.has_value()) {
        return;
    }

    SendMessage(MessageType::THERMOSTAT_RELAY_TEMPERATURE, receiverMac.value(), relayTemperature);
}

// SET_RELAY_TEMPERATURE_DELTA;00:00:00:00:00:00;value$
// SET_RELAY_TEMPERATURE_DELTA;name;value$
// SET_RELAY_TEMPERATURE_DELTA;ALL;value$
void SetRelayTemperatureDelta(const String& command) {
    std::vector<String> parts = SplitString(command, ';');
    if (parts.size() != 3) {
        return;
    }

    const String& relyTemperatureDeltaString = parts[2];
    if (relyTemperatureDeltaString.length() == 0) {
        return;
    }

    const float relayTemperatureDelta = relyTemperatureDeltaString.toFloat();

    if (parts[1] == "ALL") {
        std::set<Mac> thermostatRelays = ListThermostatRelays();
        for (const Mac& thermostatRelay : thermostatRelays) {
            SendMessage(MessageType::THERMOSTAT_RELAY_TEMPERATURE_DELTA, thermostatRelay, relayTemperatureDelta);
        }

        return;
    }

    std::optional<Mac> receiverMac = GetReceiverMac(parts[1]);
    if (!receiverMac.has_value()) {
        return;
    }

    SendMessage(MessageType::THERMOSTAT_RELAY_TEMPERATURE_DELTA, receiverMac.value(), relayTemperatureDelta);
}
