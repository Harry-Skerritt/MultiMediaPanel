//
// Created by Harry Skerritt on 12/04/2026.
//

#include "DisplayManager.h"


DisplayManager::DisplayManager(const uint8_t width, const uint8_t height)
    : m_display(width, height, &Wire, -1), m_width(width), m_height(height) {}

bool DisplayManager::initialise() {
    if(!m_display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        return false;
    }

    m_display.clearDisplay();
    m_display.setTextColor(SSD1306_WHITE);
    m_display.display();

    return true;
}

void DisplayManager::update(bool connected, char lastKey, int volume) {
    if (m_is_sleeping) return;


    // Todo: Replace
    if (m_message_timer > 0 && (millis() - m_message_timer < m_message_duration)) {
        return;
    } else {
        m_message_timer = 0;
    }

    m_display.clearDisplay();


    // Status Bar
    m_display.setTextSize(1);
    m_display.setCursor(0, 0);
    m_display.print(connected ? "BT: OK" : "BT: pairing...");
    m_display.drawLine(0, 10, 128, 10, SSD1306_WHITE);

    // Main Info
    m_display.setCursor(0, 20);
    m_display.setTextSize(2);
    m_display.print("Key: ");
    m_display.print(lastKey);

    m_display.setCursor(0, 45);
    m_display.setTextSize(1);
    m_display.print("Volume: ");
    m_display.print(volume);

    m_display.display();
}

void DisplayManager::showMessage(const char* title, const char* msg, uint32_t duration) {
    m_display.clearDisplay();
    m_display.setTextSize(1);
    m_display.setCursor(0, 10);
    m_display.println(title);
    m_display.setTextSize(2);
    m_display.println(msg);
    m_display.display();

    m_message_timer = millis();
    m_message_duration = duration;
}

void DisplayManager::setSleep(const bool active) {
    m_is_sleeping = active;

    if (m_is_sleeping) {
        m_display.ssd1306_command(SSD1306_DISPLAYOFF);
    } else {
        m_display.ssd1306_command(SSD1306_DISPLAYON);
    }
}