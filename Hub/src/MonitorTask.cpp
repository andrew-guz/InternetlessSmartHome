#include "../include/MonitorTask.h"

#include <Arduino.h>
#include <LCDI2C_Multilingual.h>
#include <set>

#include "../include/Data.h"

LCDI2C_Generic lcd(0x27, 20, 4);

void MonitorTaskInit() {
    lcd.init();
    lcd.backlight();

    lcd.clear();
    lcd.setCursor(7, 1);
    lcd.print("Hello!");
}

void MonitorTask(void* pvParameters) {
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1));

        std::set<String> thermometers = ListThermometers();
        std::set<String> relays = ListRelays();
        std::set<String> thermostatRelays = ListThermostatRelays();

        // Right now this is simple Monitor so let it be simple

        for (const String& thermometer : thermometers) {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(thermometer.substring(4));
            lcd.print(": ");
            lcd.print(GetThermometerValue(thermometer));

            vTaskDelay(pdMS_TO_TICKS(3000));
        }

        for (const String& relay : relays) {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(relay.substring(4));
            lcd.print(": ");
            lcd.print(GetRelayState(relay));

            vTaskDelay(pdMS_TO_TICKS(3000));
        }

        for (const String& thermostatRelay : thermostatRelays) {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(thermostatRelay.substring(4));
            lcd.print(": ");
            lcd.print(GetThermostatRelayState(thermostatRelay));

            vTaskDelay(pdMS_TO_TICKS(3000));
        }
    }
}
