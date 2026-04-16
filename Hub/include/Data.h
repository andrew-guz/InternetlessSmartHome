#ifndef _DATA_H_
#define _DATA_H_

#include <Arduino.h>
#include <WString.h>
#include <map>
#include <set>

extern SemaphoreHandle_t mutex;

extern std::set<String> thermometers;
extern std::set<String> relays;
extern std::set<String> thermostats;

extern std::map<String, float> thermometerValues;
extern std::map<String, bool> relayState;

void ClearData();

void AddThermometer(const String& ssid);

void AddRelay(const String& ssid);

void AddThermostat(const String& ssid);

void SetThermometerValue(const String& ssid, float value);

void SetRelayState(const String& ssid, bool state);

float GetThermometerValue(const String& ssid);

bool GetRelayState(const String& ssid);

#endif // _DATA_H_
