//
// Created by Harry Skerritt on 12/04/2026.
//

#include "PageManager.h"

PageManager::PageManager(LEDManager& leds, BleKeyboard& keyboard)
    : m_settings_page(leds) {
    m_current_page = &m_settings_page; // Todo: Change eventually
}

void PageManager::switchPage(const PageID new_page) {
    switch (new_page) {
        case PageID::MEDIA:    m_current_page = nullptr; break;
        case PageID::SETTINGS: m_current_page = &m_settings_page; break;
    }
}

void PageManager::update(const EncoderAction action, const char key) const {
    if (m_current_page) {
        m_current_page->handleInput(action, key);
    }
}

void PageManager::draw(Adafruit_SSD1306& display) const {
    if (m_current_page) {
        m_current_page->draw(display);
    }
}
