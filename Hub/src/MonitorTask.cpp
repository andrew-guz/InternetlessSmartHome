#include "../include/MonitorTask.hpp"

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <set>

#include "./include/Data.hpp"
#include "./include/Messages.hpp"

LiquidCrystal_I2C lcd(0x27, 20, 4);

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

        std::set<Mac> thermometers = ListThermometers();
        std::set<Mac> relays = ListRelays();
        std::set<Mac> thermostatRelays = ListThermostatRelays();

        // Right now this is simple Monitor so let it be simple

        const auto showData = [](const Mac mac, const std::uint8_t row, auto value) {
            lcd.setCursor(0, row);
            lcd.print(GetName(mac));
            lcd.setCursor(0, row + 1);
            lcd.print(value);
        };

        std::uint8_t row = 0;
        for (auto iter = thermometers.begin(); iter != thermometers.end(); ++iter, row += 2) {
            lcd.clear();

            if (row == 4) {
                row = 0;
            }

            showData(*iter, 0, GetThermometerValue(*iter));

            vTaskDelay(pdMS_TO_TICKS(3000));
        }

        row = 0;
        for (auto iter = relays.begin(); iter != relays.end(); ++iter, row += 2) {
            lcd.clear();

            if (row == 4) {
                row = 0;
            }

            showData(*iter, 0, GetRelayState(*iter));

            vTaskDelay(pdMS_TO_TICKS(3000));
        }

        row = 0;
        for (auto iter = thermostatRelays.begin(); iter != thermostatRelays.end(); ++iter, row += 2) {
            lcd.clear();

            if (row == 4) {
                row = 0;
            }

            showData(*iter, 0, GetThermostatRelayState(*iter).temperatureState);

            vTaskDelay(pdMS_TO_TICKS(3000));
        }
    }
}
