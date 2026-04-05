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

class WiFiDualServer : public WiFiDataServer {
public:
    virtual void Setup(const String& ssid, const String& password, const std::uint8_t network) override {
        WiFi.mode(WIFI_AP_STA);

        IPAddress localIP(192, 168, network, 1);
        WiFi.softAPConfig(localIP, localIP, IPAddress(255, 255, 255, 0));

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
        String fullUrl = String("http://") + targetApIp.toString() + url;

        HTTPClient http;
        http.setTimeout(3000);
        WiFiClient client;
        String body;

        if (http.begin(client, fullUrl)) {
            const char* headers[] = { "WWW-Authenticate" };
            http.collectHeaders(headers, 1);
            int httpCode = http.GET();
            if (httpCode == 401) {
                String authReq = http.header("WWW-Authenticate");
                http.end();

                if (authReq.length() > 0) {
                    const String auth = BuildDigestAuth(authReq, url);

                    if (http.begin(client, fullUrl)) {
                        http.addHeader("Authorization", auth);
                        httpCode = http.GET();

                        if (httpCode == HTTP_CODE_OK) {
                            body = http.getString();
                        }

                        http.end();
                    }
                }
            } else if (httpCode == HTTP_CODE_OK) {
                body = http.getString();

                http.end();
            } else {
                http.end();
            }
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

private:
    String ExtractDigestParam(const String& authRequest, const String& param) {
        int beginIdx = authRequest.indexOf(param + "=\"");
        if (beginIdx == -1)
            return "";
        beginIdx += param.length() + 2;
        int endIdx = authRequest.indexOf("\"", beginIdx);
        return authRequest.substring(beginIdx, endIdx);
    }

    String MD5String(String input) {
        MD5Builder md5;
        md5.begin();
        md5.add(input);
        md5.calculate();
        return md5.toString();
    }

    String BuildDigestAuth(const String& authRequest, const String& uri) {
        String realm = ExtractDigestParam(authRequest, "realm");
        String nonce = ExtractDigestParam(authRequest, "nonce");
        String qop = ExtractDigestParam(authRequest, "qop");

        String cnonce = "00000001";
        String nc = "00000001";

        // Используем предвычисленный _hash как HA1
        // _hash уже содержит MD5(username:realm:password)

        // HA2 = MD5(GET:uri)
        String ha2 = MD5String("GET:" + uri);

        // response = MD5(HA1:nonce:nc:cnonce:qop:HA2)
        String response = MD5String(_hash + ":" + nonce + ":" + nc + ":" + cnonce + ":" + qop + ":" + ha2);

        return "Digest username=\"" + String{ WWW_USER_NAME } + "\", realm=\"" + realm + "\", " + "nonce=\"" + nonce + "\", uri=\"" + uri +
               "\", qop=" + qop + ", " + "nc=" + nc + ", cnonce=\"" + cnonce + "\", response=\"" + response + "\"";
    }
};
