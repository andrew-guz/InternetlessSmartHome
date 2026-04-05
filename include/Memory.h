#pragma once

#include <Preferences.h>
#include <WString.h>

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
