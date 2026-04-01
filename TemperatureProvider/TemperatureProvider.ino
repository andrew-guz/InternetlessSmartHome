#include <Arduino.h>
// #include <ArduinoOTA.h>

#include "Display.h"
#include "Temperature.h"
#include "WiFi.h"

#define SSID     String("Thermometer")
#define PASSWORD String("1234567890")

OLEDDisplay display;
I2CTemperatureSensor temperature;
WiFiDataServer<float> server;

void setup() {
    display.Setup();
    temperature.Setup();
    server.Setup(SSID, PASSWORD);

    display.ShowString(server.GetAddress().toString());
}

void loop() { server.Loop(); }
