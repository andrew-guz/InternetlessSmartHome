#pragma once

#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <IPAddress.h>
#include <WString.h>

template<typename T>
class WiFiDataServer {
public:
    void Setup(const String& ssid, const String& password) {
        WiFi.softAP(ssid, password);

        _ipAddress = WiFi.softAPIP();

        _server.on("/get", HTTPMethod::HTTP_GET, std::bind(&WiFiDataServer<T>::HandleGet, this));

        _server.begin();
    }

    IPAddress GetAddress() { return _ipAddress; }

    void SetData(T data) { _data = data; }

    void Loop() { _server.handleClient(); }

private:
    void HandleGet() { _server.send(200, "text/plain", String(_data)); }

private:
    ESP8266WebServer _server{ 80 };
    IPAddress _ipAddress;
    T _data = 0;
};
