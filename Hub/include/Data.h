#ifndef _DATA_H_
#define _DATA_H_

#include <Arduino.h>
#include <WString.h>
#include <map>
#include <set>

extern SemaphoreHandle_t mutex;

extern std::set<String> thermometers;
extern std::set<String> relays;
extern std::set<String> thermostatRelays;

extern std::map<String, float> thermometerValues;
extern std::map<String, bool> relayState;
extern std::map<String, bool> thermostatRelayState;

void ClearData();

std::set<String> ListThermometers();

std::set<String> ListRelays();

std::set<String> ListThermostatRelays();

void AddThermometer(const String& ssid);

void AddRelay(const String& ssid);

void AddThermostatRelay(const String& ssid);

void SetThermometerValue(const String& ssid, float value);

void SetRelayState(const String& ssid, bool state);

void SetThermostatRelayState(const String& ssid, bool state);

float GetThermometerValue(const String& ssid);

bool GetRelayState(const String& ssid);

bool GetThermostatRelayState(const String& ssid);

#endif // _DATA_H_
