#include "../include/Data.hpp"

#include <Arduino.h>
#include <WString.h>
#include <map>

#include "../include/Messages.hpp"

SemaphoreHandle_t mutex = nullptr;
extern QueueHandle_t espNowMessagesQueue = nullptr;

std::map<Mac, float> thermometerValues;
std::map<Mac, bool> relayStates;
std::map<Mac, RelayState> thermostatRelayStates;

std::unordered_map<Mac, String, MacHash> names;

void LoadData() {
    // TODO load from memory
}

std::set<Mac> ListThermometers() {
    xSemaphoreTake(mutex, portMAX_DELAY);

    std::set<Mac> result;
    for (const auto& pair : thermometerValues) {
        result.insert(pair.first);
    }

    xSemaphoreGive(mutex);

    return result;
}

std::set<Mac> ListRelays() {
    xSemaphoreTake(mutex, portMAX_DELAY);

    std::set<Mac> result;
    for (const auto& pair : relayStates) {
        result.insert(pair.first);
    }

    xSemaphoreGive(mutex);

    return result;
}

std::set<Mac> ListThermostatRelays() {
    xSemaphoreTake(mutex, portMAX_DELAY);

    std::set<Mac> result;
    for (const auto& pair : thermostatRelayStates) {
        result.insert(pair.first);
    }

    xSemaphoreGive(mutex);

    return result;
}

void SetThermometerValue(const Mac& mac, float value) {
    xSemaphoreTake(mutex, portMAX_DELAY);

    thermometerValues[mac] = value;

    xSemaphoreGive(mutex);
}

void SetRelayState(const Mac& mac, bool state) {
    xSemaphoreTake(mutex, portMAX_DELAY);

    relayStates[mac] = state;

    xSemaphoreGive(mutex);
}

void SetThermostatRelayState(const Mac& mac, const RelayState& state) {
    xSemaphoreTake(mutex, portMAX_DELAY);

    thermostatRelayStates[mac] = state;

    xSemaphoreGive(mutex);
}

float GetThermometerValue(const Mac& mac) {
    xSemaphoreTake(mutex, portMAX_DELAY);

    const float value = thermometerValues[mac];

    xSemaphoreGive(mutex);

    return value;
}

bool GetRelayState(const Mac& mac) {
    xSemaphoreTake(mutex, portMAX_DELAY);

    const bool state = relayStates[mac];

    xSemaphoreGive(mutex);

    return state;
}

RelayState GetThermostatRelayState(const Mac& mac) {
    xSemaphoreTake(mutex, portMAX_DELAY);

    const RelayState state = thermostatRelayStates[mac];

    xSemaphoreGive(mutex);

    return state;
}

void SetName(const Mac& mac, const String& name) {
    xSemaphoreTake(mutex, portMAX_DELAY);

    names[mac] = name;

    xSemaphoreGive(mutex);
}

String GetName(const Mac& mac) {
    xSemaphoreTake(mutex, portMAX_DELAY);

    String name;
    auto iter = names.find(mac);
    if (iter != names.end())
        name = iter->second;

    xSemaphoreGive(mutex);

    return name;
}
