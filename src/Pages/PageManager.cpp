//
// Created by Harry Skerritt on 12/04/2026.
//

#include "PageManager.h"

PageManager::PageManager(LEDManager& leds, BleKeyboard& keyboard, SettingsManager& settings_manager)
    : m_leds(leds), m_current_page_id(), m_settings_page(leds, settings_manager), m_media_page(leds, keyboard) {
    m_current_page = &m_media_page; // Todo: Change eventually
}

void PageManager::initialise(const DeviceSettings &settings) {
    m_settings_page.syncSettings(settings);
}



PageID PageManager::update(const EncoderAction action, const char key) {
    if (m_current_page) {
        const PageID next_page = m_current_page->handleInput(action, key);

        if (next_page != m_current_page_id) {
            switchPage(next_page);
        }
        return next_page;
    }
    return PageID::MEDIA;
}

void PageManager::switchPage(const PageID new_page) {
    m_current_page_id = new_page;
    switch (new_page) {
        case PageID::MEDIA:    m_current_page = &m_media_page; break;
        case PageID::SETTINGS: m_current_page = &m_settings_page; break;
    }

    m_leds.fill(m_current_page->getPageTheme());
}

void PageManager::draw(Adafruit_SSD1306& display) const {
    if (m_current_page) {
        m_current_page->draw(display);
    }
}
