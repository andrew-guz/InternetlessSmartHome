#pragma once

#include <array>
#include <cstdint>

typedef std::array<std::uint8_t, 6> Mac;

enum class MessageType : std::uint8_t {
    TEMPERATURE,             // sent by thermometer
    THERMOMETER_BRIGHTNESS,  // send to thermometer to configure brightness
    RELAY_STATE,             // sent by relay - full relay state
    RELAY_MANUAL_MODE,       // send to relay to trigger manual mode
    RELAY_MANUAL_STATE,      // send to relay to trigger relay in manual mode
    RELAY_THERMOMETER,       // send to relay to specify exact thermometer to listen to
    RELAY_TEMPERATURE,       // send to relay to specify threshold temperature
    RELAY_TEMPERATURE_DELTA, // send to relay to specify temperature delta
};

#define DATA_SIZE 128

struct Message {
    Mac sender;
    MessageType type;
    char data[DATA_SIZE];
    std::uint8_t dataSize;
};
