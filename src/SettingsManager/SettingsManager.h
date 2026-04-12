//
// Created by Harry Skerritt on 12/04/2026.
//

#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H
#pragma once
#include <Preferences.h>

struct DeviceSettings {
    uint8_t led_brightness;
    bool leds_enabled;
    uint8_t screen_contrast;
    int sleep_mins;
};

class SettingsManager {
public:
    void begin();

    void save(const DeviceSettings& settings);
    DeviceSettings load();

    void clearAll();

private:
    Preferences m_prefs;
};




#endif //SETTINGSMANAGER_H
