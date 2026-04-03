#pragma once

#include <EEPROM.h>

template<typename T>
class Memory {
public:
    void Setup() { EEPROM.begin(sizeof(T)); }

    void Save(const T& data) {
        EEPROM.put(0, data);
        EEPROM.commit();
    }

    T Load() {
        T data;
        EEPROM.get(0, data);
        return data;
    }
};
