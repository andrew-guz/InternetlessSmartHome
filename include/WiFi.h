#pragma once

#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiType.h>
#include <IPAddress.h>
#include <WString.h>
#include <functional>
#include <optional>

#include "Defines.h"

class WiFiDataServer {
public:
    WiFiDataServer() { _hash = _server.credentialHash(WWW_USER_NAME, WWW_REALM, WWW_PASSWORD); }

    virtual void Setup(const String& ssid, const String& password) {
        WiFi.mode(WIFI_AP);

        IPAddress localIP(192, 168, NETWORK, 1);

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

class WiFiDualServer : public WiFiDataServer {
public:
    virtual void Setup(const String& ssid, const String& password) override {
        WiFi.mode(WIFI_AP_STA);
        WiFi.softAP(ssid, password);

        _ipAddress = WiFi.softAPIP();

        _server.begin();
    }

    template<typename T>
    std::optional<T> GetData(const String& ssid, const String& url) {
        WiFi.begin(ssid, WIFI_PASSWORD);

        int attempts = 30;
        while (WiFi.status() != WL_CONNECTED && attempts > 0) {
            delay(100);
            _server.handleClient();
            --attempts;
        }

        if (WiFi.status() != WL_CONNECTED)
            return std::nullopt;

        IPAddress targetApIp = WiFi.gatewayIP();

        String body;
        HTTPClient http;
        http.setTimeout(3000);
        WiFiClient client;
        String fullUrl = String("http://") + targetApIp.toString() + url;
        if (http.begin(client, fullUrl)) {
            if (http.GET() == HTTP_CODE_OK) {
                body = http.getString();
            }

            http.end();
        }

        WiFi.disconnect();

        if (body.length() == 0)
            return std::nullopt;

        if constexpr (std::is_same_v<T, String>)
            return body;
        else if constexpr (std::is_same_v<T, int>)
            return body.toInt();
        else if constexpr (std::is_same_v<T, float>)
            return body.toFloat();
        else if constexpr (std::is_same_v<T, bool>)
            return body == "true" ? true : false;

        return std::nullopt;
    }
};
