//
// Created by Harry Skerritt on 12/04/2026.
//

#include "SettingsManager.h"

void SettingsManager::begin() {
    m_prefs.begin("macropad", false);
}

void SettingsManager::save(const DeviceSettings &settings) {
    m_prefs.putUChar("led_br", settings.led_brightness);
    m_prefs.putBool("led_en", settings.leds_enabled);
    m_prefs.putUChar("scr_ct", settings.screen_contrast);
    m_prefs.putInt("slp_mn", settings.sleep_mins);
}

DeviceSettings SettingsManager::load() {
    DeviceSettings settings;

    settings.led_brightness = m_prefs.getUChar("led_br", 127);
    settings.leds_enabled = m_prefs.getBool("led_en", true);
    settings.screen_contrast = m_prefs.getUChar("scr_ct", 127);
    settings.sleep_mins = m_prefs.getInt("slp_mn", 5);
    return settings;
}

void SettingsManager::clearAll() {
    m_prefs.clear();
}


