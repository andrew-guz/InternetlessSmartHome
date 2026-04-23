#pragma once

#include <WString.h>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>

typedef std::array<std::uint8_t, 6> Mac;

struct MacHash {
    std::size_t operator()(const Mac& mac) const noexcept {
        std::size_t hash = 0;
        for (auto b : mac) {
            hash = (hash * 31) ^ b;
        }
        return hash;
    }
};

inline bool operator==(const Mac& lhs, const Mac& rhs) { return memcmp(lhs.data(), rhs.data(), sizeof(Mac)) == 0; }

inline bool operator!=(const Mac& lhs, const Mac& rhs) { return memcmp(lhs.data(), rhs.data(), sizeof(Mac)) != 0; }

inline String MacToString(const Mac& mac) {
    char buffer[18];
    snprintf(buffer, sizeof(buffer), "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return String(buffer);
}

inline String MacToShortString(const Mac& mac) {
    char buffer[13];
    snprintf(buffer, sizeof(buffer), "%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return String(buffer);
}

inline Mac MacFromString(const String& str) {
    Mac mac;

    memset(mac.data(), 0, mac.size());

    if (str.length() != 17) {
        return mac;
    }

    for (std::size_t i = 0; i < mac.size(); ++i) {
        String byteStr = str.substring(i * 3, (i * 3) + 2);
        mac[i] = (std::uint8_t)strtol(byteStr.c_str(), NULL, 16);
    }

    return mac;
}

inline Mac MacFromShortString(const String& str) {
    Mac mac;

    memset(mac.data(), 0, mac.size());

    if (str.length() != 12) {
        return mac;
    }

    for (std::size_t i = 0; i < mac.size(); ++i) {
        String byteStr = str.substring(i * 2, (i * 2) + 2);
        mac[i] = (std::uint8_t)strtol(byteStr.c_str(), NULL, 16);
    }

    return mac;
}
