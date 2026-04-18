#pragma once

#include <WString.h>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>

typedef std::array<std::uint8_t, 6> Mac;

struct MacHash {
    std::size_t operator()(const Mac& mac) const noexcept {
        std::size_t hash = 0;
        for (auto b : mac) {
            hash = (hash * 31) ^ b;
        }
        return hash;
    }
};

inline bool operator==(const Mac& lhs, const Mac& rhs) { return memcmp(lhs.data(), rhs.data(), sizeof(Mac)) == 0; }

inline bool operator!=(const Mac& lhs, const Mac& rhs) { return memcmp(lhs.data(), rhs.data(), sizeof(Mac)) != 0; }

inline String MacToString(const Mac& mac) {
    char buffer[18];
    snprintf(buffer, sizeof(buffer), "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return String(buffer);
}

enum class MessageType : std::uint8_t {
    TEMPERATURE,                        // sent by thermometer
    THERMOMETER_BRIGHTNESS,             // send to thermometer to configure brightness
    RELAY_STATE,                        // sent by relay
    RELAY_SET_STATE,                    // send to relay to set state
    THERMOSTAT_RELAY_STATE,             // sent by thermostat relay
    THERMOSTAT_RELAY_MANUAL_MODE,       // send to thermostat relay to trigger manual mode
    THERMOSTAT_RELAY_MANUAL_STATE,      // send to thermostat relay to trigger relay in manual mode
    THERMOSTAT_RELAY_THERMOMETER,       // send to thermostat relay to specify exact thermometer to listen to
    THERMOSTAT_RELAY_TEMPERATURE,       // send to thermostat relay to specify threshold temperature
    THERMOSTAT_RELAY_TEMPERATURE_DELTA, // send to thermostat relay to specify temperature delta
};

typedef std::array<std::uint8_t, 128> MessageData;

struct Message {
    Mac sender;
    Mac receiver;
    MessageType type;
    MessageData data;
    std::uint8_t dataSize;
};

template<typename T>
inline void setMessageData(Message& message, const T& value) {
    memcpy(message.data.data(), &value, sizeof(T));
    message.dataSize = sizeof(T);
}

template<typename T>
inline T getMessageData(const Message& message) {
    T value;
    memcpy(&value, message.data.data(), sizeof(T));
    return value;
}

template<typename T>
inline T getMessageData(const Message* message) {
    T value;
    memcpy(&value, message->data.data(), sizeof(T));
    return value;
}

struct RelayState {
    bool manualMode;
    bool manualState;
    Mac thermometer;
    float targetTemperature;
    float temperatureDelta;
    bool temperatureState;
};
