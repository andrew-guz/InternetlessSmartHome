#pragma once

#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiType.h>
#include <IPAddress.h>
#include <WString.h>
#include <functional>

class WiFiDataServer {
public:
    virtual void Setup(const String& ssid, const String& password) {
        WiFi.mode(WIFI_AP);
        WiFi.softAP(ssid);
        _server.begin();
    }

    struct Response {
        int code;
        String contentType;
        String content;
    };

    void Register(const String& path, HTTPMethod method, std::function<WiFiDataServer::Response(void)> handler) {
        _server.on(path, method, [this, handler]() {
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
};
