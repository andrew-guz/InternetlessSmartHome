#pragma once

#include <array>
#include <cstdint>

typedef std::array<std::uint8_t, 6> Mac;

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

#define DATA_SIZE 128

struct Message {
    Mac sender;
    Mac receiver;
    MessageType type;
    char data[DATA_SIZE];
    std::uint8_t dataSize;
};
