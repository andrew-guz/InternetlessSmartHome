#ifndef _DATA_H_
#define _DATA_H_

#include <Arduino.h>
#include <WString.h>
#include <map>
#include <set>
#include <unordered_map>

#include "Messages.hpp"

extern SemaphoreHandle_t mutex;

extern std::map<Mac, float> thermometerValues;
extern std::map<Mac, bool> relayStates;
extern std::map<Mac, RelayState> thermostatRelayStates;

extern std::unordered_map<Mac, String, MacHash> names;

void LoadData();

std::set<Mac> ListThermometers();

std::set<Mac> ListRelays();

std::set<Mac> ListThermostatRelays();

void SetThermometerValue(const Mac& mac, float value);

void SetRelayState(const Mac& mac, bool state);

void SetThermostatRelayState(const Mac& mac, const RelayState& state);

float GetThermometerValue(const Mac& mac);

bool GetRelayState(const Mac& mac);

RelayState GetThermostatRelayState(const Mac& mac);

void SetName(const Mac& mac, const String& name);

String GetName(const Mac& mac);

#endif // _DATA_H_
