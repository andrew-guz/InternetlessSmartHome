#pragma once

#include <Preferences.h>
#include <WString.h>
#include <unordered_map>
#include <vector>

#include "Mac.hpp"
#include "Utils.hpp"

class Memory {
public:
    void Setup() { _preferences.begin("IMS", false); }

    template<typename T>
    void Save(const char* name, const T& data) = delete;

    template<typename T>
    T Load(const char* name, T defaultValue) = delete;

private:
    Preferences _preferences;
};

template<>
inline void Memory::Save<float>(const char* name, const float& data) {
    _preferences.putFloat(name, data);
}

template<>
inline float Memory::Load<float>(const char* name, float defaultValue) {
    return _preferences.getFloat(name, defaultValue);
}

template<>
inline void Memory::Save<int>(const char* name, const int& data) {
    _preferences.putInt(name, data);
}

template<>
inline int Memory::Load<int>(const char* name, int defaultValue) {
    return _preferences.getInt(name, defaultValue);
}

template<>
inline void Memory::Save<String>(const char* name, const String& data) {
    _preferences.putString(name, data);
}

template<>
inline String Memory::Load<String>(const char* name, String defaultValue) {
    return _preferences.getString(name, defaultValue);
}

template<>
inline void Memory::Save<bool>(const char* name, const bool& data) {
    _preferences.putBool(name, data);
}

template<>
inline bool Memory::Load<bool>(const char* name, bool defaultValue) {
    return _preferences.getBool(name, defaultValue);
}

template<>
inline void Memory::Save<Mac>(const char* name, const Mac& data) {
    _preferences.putBytes(name, data.data(), 6);
}

template<>
inline Mac Memory::Load<Mac>(const char* name, Mac defaultValue) {
    Mac data;
    if (_preferences.getBytes(name, data.data(), 6) != 6)
        return defaultValue;

    return data;
}

template<>
inline void Memory::Save<std::unordered_map<Mac, String, MacHash>>(const char* name, const std::unordered_map<Mac, String, MacHash>& data) {
    String macKeysString;
    for (const auto& [mac, deviceName] : data) {
        String macString = MacToShortString(mac); // 12 symbols
        String key = "m/" + macString;            // must be < 15 symbols
        macKeysString += key + ";";
        _preferences.putString(key.c_str(), deviceName);
    }

    _preferences.putString(name, macKeysString);
}

template<>
inline std::unordered_map<Mac, String, MacHash>
Memory::Load<std::unordered_map<Mac, String, MacHash>>(const char* name, std::unordered_map<Mac, String, MacHash> defaultValue) {
    std::unordered_map<Mac, String, MacHash> data;

    String macKeysString = _preferences.getString(name);
    if (macKeysString.isEmpty())
        return defaultValue;

    std::vector<String> parts = SplitString(macKeysString, ';');
    for (const String& part : parts) {
        Mac mac = MacFromShortString(part);
        String name = _preferences.getString(part.c_str());
        data[mac] = name;
    }

    return data;
}
