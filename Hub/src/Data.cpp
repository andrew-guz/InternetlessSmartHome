#include "../include/Data.h"

#include <Arduino.h>
#include <WString.h>
#include <map>
#include <set>
#include <string>

SemaphoreHandle_t mutex = NULL;

std::set<String> thermometers;
std::set<String> relays;
std::set<String> thermostatRelays;

std::map<String, float> thermometerValues;
std::map<String, bool> relayState;
std::map<String, bool> thermostatRelayState;

void ClearData() {
    xSemaphoreTake(mutex, portMAX_DELAY);

    thermometers.clear();
    relays.clear();
    thermostatRelays.clear();
    thermometerValues.clear();
    relayState.clear();

    xSemaphoreGive(mutex);
}

std::set<String> ListThermometers() {
    xSemaphoreTake(mutex, portMAX_DELAY);

    std::set<String> result = thermometers;

    xSemaphoreGive(mutex);

    return result;
}

std::set<String> ListRelays() {
    xSemaphoreTake(mutex, portMAX_DELAY);

    std::set<String> result = relays;

    xSemaphoreGive(mutex);

    return result;
}

std::set<String> ListThermostatRelays() {
    xSemaphoreTake(mutex, portMAX_DELAY);

    std::set<String> result = thermostatRelays;

    xSemaphoreGive(mutex);

    return result;
}

void AddThermometer(const String& ssid) {
    xSemaphoreTake(mutex, portMAX_DELAY);

    thermometers.insert(ssid);
    thermometerValues[ssid] = 0;

    xSemaphoreGive(mutex);
}

void AddRelay(const String& ssid) {
    xSemaphoreTake(mutex, portMAX_DELAY);

    relays.insert(ssid);
    relayState[ssid] = false;

    xSemaphoreGive(mutex);
}

void AddThermostatRelay(const String& ssid) {
    xSemaphoreTake(mutex, portMAX_DELAY);

    thermostatRelays.insert(ssid);

    xSemaphoreGive(mutex);
}

void SetThermometerValue(const String& ssid, float value) {
    xSemaphoreTake(mutex, portMAX_DELAY);

    thermometerValues[ssid] = value;

    xSemaphoreGive(mutex);
}

void SetRelayState(const String& ssid, bool state) {
    xSemaphoreTake(mutex, portMAX_DELAY);

    relayState[ssid] = state;

    xSemaphoreGive(mutex);
}

void SetThermostatRelayState(const String& ssid, bool state) {
    xSemaphoreTake(mutex, portMAX_DELAY);

    thermostatRelayState[ssid] = state;

    xSemaphoreGive(mutex);
}

float GetThermometerValue(const String& ssid) {
    xSemaphoreTake(mutex, portMAX_DELAY);

    const float value = thermometerValues[ssid];

    xSemaphoreGive(mutex);

    return value;
}

bool GetRelayState(const String& ssid) {
    xSemaphoreTake(mutex, portMAX_DELAY);

    const bool state = relayState[ssid];

    xSemaphoreGive(mutex);

    return state;
}

bool GetThermostatRelayState(const String& ssid) {
    xSemaphoreTake(mutex, portMAX_DELAY);

    const bool state = thermostatRelayState[ssid];

    xSemaphoreGive(mutex);

    return state;
}
