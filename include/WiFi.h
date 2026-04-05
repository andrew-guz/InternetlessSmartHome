#pragma once

#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiType.h>
#include <IPAddress.h>
#include <WString.h>
#include <cstdint>
#include <functional>
#include <optional>

#include "Defines.h"

class WiFiDataServer {
public:
    WiFiDataServer() { _hash = _server.credentialHash(WWW_USER_NAME, WWW_REALM, WWW_PASSWORD); }

    virtual void Setup(const String& ssid, const String& password, const std::uint8_t network) {
        WiFi.mode(WIFI_AP);

        IPAddress localIP(192, 168, network, 1);
        WiFi.softAPConfig(localIP, localIP, IPAddress(255, 255, 255, 0));

        WiFi.softAP(ssid);

        _ipAddress = WiFi.softAPIP();

        _server.begin();
    }

    IPAddress GetAddress() { return _ipAddress; }

    struct Response {
        int code;
        String contentType;
        String content;
    };

    void Register(const String& path, HTTPMethod method, std::function<WiFiDataServer::Response(void)> handler) {
        _server.on(path, method, [this, handler]() {
            if (!_server.authenticateDigest(WWW_USER_NAME, _hash)) {
                _server.requestAuthentication(DIGEST_AUTH, WWW_REALM, "Authentication failed");
                return;
            }

            WiFiDataServer::Response response = handler();
            _server.send(response.code, response.contentType, response.content);
        });
    }

    void Register(const String& path, HTTPMethod method, std::function<WiFiDataServer::Response(const String&)> handler) {
        _server.on(path, method, [this, handler]() {
            WiFiDataServer::Response response = handler(_server.arg("plain"));
            _server.send(response.code, response.contentType, response.content);
        });
    }

    void Loop() { _server.handleClient(); }

protected:
    ESP8266WebServer _server{ 80 };
    String _hash;
    IPAddress _ipAddress;
};
