//
// Created by Harry Skerritt on 12/04/2026.
//

#include "SettingsPage.h"

SettingsPage::SettingsPage(LEDManager &leds, SettingsManager& settings_manager)
    : m_leds(leds), m_settings_manager(settings_manager)
{
    //leds.fill(RGBColour(255, 100, 0));
}

void SettingsPage::syncSettings(const DeviceSettings &settings) {
    m_leds_enabled = settings.leds_enabled;
    m_brightness = settings.led_brightness;
    m_contrast = settings.screen_contrast;
    m_sleep_minutes = settings.sleep_mins;

    Serial.println("SettingsPage Synced!");
}


PageID SettingsPage::handleInput(const EncoderAction action, char key) {
    if (action == EncoderAction::NONE) return PageID::SETTINGS;

    if (m_confirm_reset) {
        if (action == EncoderAction::CLOCKWISE || action == EncoderAction::COUNTER_CLOCKWISE) {
            m_reset_choice = !m_reset_choice;
        }

        if (action == EncoderAction::SINGLE_CLICK) {
            if (m_reset_choice) {
                // Yes Reset
                m_settings_manager.clearAll();
                delay(500);
                ESP.restart();
            } else {
                // No dont reset
                m_confirm_reset = false;
            }
        }
        return PageID::SETTINGS;
    }

    if (action == EncoderAction::SINGLE_CLICK) {
        if (m_menu_index == m_max_items - 1) { // Back
            saveToFlash();
            return PageID::MEDIA;
        }

        if (m_menu_index == 0) { // Setup Pages
            return PageID::SETTINGS;
        }

        if (m_menu_index == 5) { // Reset BT
           m_confirm_reset = true;
            m_reset_choice = false;
            return PageID::SETTINGS;
        }


        if (m_is_editing) {
            saveToFlash();
        }

        m_is_editing = !m_is_editing;
        m_leds.onInteraction(m_is_editing ? COLOUR_WHITE : COLOUR_GREEN, 200);
        return PageID::SETTINGS;
    }

    if (m_is_editing) {
        switch (m_menu_index) {
            case 1: // Brightness
                if (action == EncoderAction::CLOCKWISE) m_brightness = min(255, m_brightness + 13);
                if (action == EncoderAction::COUNTER_CLOCKWISE) m_brightness = max(0, m_brightness - 13);
                m_leds.setBrightness(m_brightness);
                break;

            case 2: // LED State
                if (action == EncoderAction::CLOCKWISE || action == EncoderAction::COUNTER_CLOCKWISE) {
                    m_leds_enabled = !m_leds_enabled;
                    m_leds.setEnabled(m_leds_enabled);
                    //m_is_editing = false;
                }
                break;

            case 3: // Screen Contrast
                if (action == EncoderAction::CLOCKWISE) m_contrast = min(255, m_contrast + 25);
                if (action == EncoderAction::COUNTER_CLOCKWISE) m_contrast = max(0, m_contrast - 25);
                break;

            case 4: // Sleep Timer
                if (action == EncoderAction::CLOCKWISE) m_sleep_minutes++;
                if (action == EncoderAction::COUNTER_CLOCKWISE) m_sleep_minutes--;
                m_sleep_minutes = constrain(m_sleep_minutes, 1, 30);
                break;

            default: ;
        }

    } else {
        if (action == EncoderAction::CLOCKWISE) m_menu_index = (m_menu_index + 1) % m_max_items;
        if (action == EncoderAction::COUNTER_CLOCKWISE) m_menu_index = (m_menu_index + m_max_items - 1) % m_max_items;
    }
    return PageID::SETTINGS;
}


void SettingsPage::draw(Adafruit_SSD1306 &display) {
    display.ssd1306_command(SSD1306_SETCONTRAST);
    display.ssd1306_command(m_contrast);
    display.clearDisplay();

    // Header
    display.fillRect(0, 0, 128, 14, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(20, 3);
    display.print("S E T T I N G S");


    // Scroll Offset
    int view_start = 0;
    if (m_menu_index >= 4) {
        view_start = m_menu_index - 3;
    }

    // Draw Items
    for (int i = 0; i < 4; i++) {
        int item_index = view_start + i;
        if (item_index >= m_max_items) break;

        const int y_Pos = 18 + (i * 11);

        display.setTextColor(SSD1306_WHITE);

        if (item_index == m_menu_index) {
            if (m_is_editing) {
                display.fillRect(2, y_Pos - 1, 124, 10, SSD1306_WHITE);
                display.setTextColor(SSD1306_BLACK);
            } else {
                display.drawRoundRect(2, y_Pos - 1, 124, 10, 3, SSD1306_WHITE);
            }
        }

        // Draw Label
        display.setCursor(10, y_Pos);
        display.print(m_options[item_index]);

        // Draw Values
        display.setCursor(100, y_Pos);
        if (item_index == 1) { // Brightness
            display.print(map(m_brightness, 0, 255, 0, 100));
            display.print("%");
        }
        else if (item_index == 2) { // LED State
            display.print(m_leds_enabled ? "ON" : "OFF");
        }
        else if (item_index == 3) { // Contrast
            display.print(map(m_contrast, 0, 255, 0, 100));
            display.print("%");
        }
        else if (item_index == 4) { // Sleep
            display.print(m_sleep_minutes);
            display.print("m");
        }
    }


    if (m_confirm_reset) {
        // Draw Backdrop
        display.fillRect(10, 15, 108, 35, SSD1306_BLACK);
        display.drawRect(10, 15, 108, 35, SSD1306_WHITE);

        display.setTextColor(SSD1306_WHITE);
        display.setCursor(20, 20);
        display.print("ARE YOU SURE?");

        // Draw No
        if (!m_reset_choice) {
            display.fillRect(20, 32, 35, 12, SSD1306_WHITE);
            display.setTextColor(SSD1306_BLACK);
        } else {
            display.setTextColor(SSD1306_WHITE);
        }
        display.setCursor(30, 34);
        display.print("NO");

        // Draw Yes
        if (m_reset_choice) {
            display.fillRect(70, 32, 35, 12, SSD1306_WHITE);
            display.setTextColor(SSD1306_BLACK);
        } else {
            display.setTextColor(SSD1306_WHITE);
        }
        display.setCursor(80, 34);
        display.print("YES");
    }

    display.display();



    if (!m_leds_enabled) {
        m_leds.off();
    } else {
        if (m_is_editing && m_menu_index == 1) {
            m_leds.setBrightness(m_brightness);
        }
    }
}


void SettingsPage::saveToFlash() const {
    DeviceSettings current;
    current.led_brightness = m_brightness;
    current.leds_enabled = m_leds_enabled;
    current.screen_contrast = m_contrast;
    current.sleep_mins = m_sleep_minutes;

    m_settings_manager.save(current);
}


