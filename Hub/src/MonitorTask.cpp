#include "../include/MonitorTask.hpp"

#include <Arduino.h>
#include <LCDI2C_Multilingual.h>

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
    }
}
