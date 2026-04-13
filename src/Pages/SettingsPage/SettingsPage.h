//
// Created by Harry Skerritt on 12/04/2026.
//

#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H
#include "LEDManager/LEDManager.h"
#include "Pages/Base/Page.h"
#include "SettingsManager/SettingsManager.h"


class SettingsPage : public Page {

public:
    explicit SettingsPage(LEDManager& leds, SettingsManager& settings_manager);

    void draw(Adafruit_SSD1306 &display) override;
    PageID handleInput(EncoderAction action, char key) override;

    RGBColour getPageTheme() override {
        return RGBColour(255, 100, 0);
    }

    void syncSettings(const DeviceSettings& settings);

private:
    LEDManager& m_leds;
    SettingsManager& m_settings_manager;

    void saveToFlash() const;

    int m_menu_index = 0;
    bool m_is_editing = false;

    // Settings Vars
    int m_brightness = 127;
    bool m_leds_enabled = true;
    int m_sleep_minutes = 5;
    uint8_t m_contrast = 127;

    // Reset
    bool m_confirm_reset = false;
    bool m_reset_choice = false; // False = No, True = Yes


    static constexpr int m_max_items = 7;
    const char* m_options[m_max_items] = {
        "Setup Pages", "LED Brightness", "LED State",
        "OLED Contrast", "Sleep Timer", "Reset", "Back"
    };
};



#endif //SETTINGSPAGE_H
