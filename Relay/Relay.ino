#include <Arduino.h>
#include <ArduinoOTA.h>

#include "Defines.h"
#include "DeviceDefines.h"

void setup() {
    ArduinoOTA.setHostname(DEVICE_OTA_NAME);
    ArduinoOTA.setPasswordHash(OTA_PASSWORD);
    ArduinoOTA.begin();
}

void loop() {
    ArduinoOTA.handle();
    delay(1);
}
