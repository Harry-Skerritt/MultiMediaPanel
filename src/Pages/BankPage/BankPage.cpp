//
// Created by Harry Skerritt on 13/04/2026.
//

#include "BankPage.h"

BankPage::BankPage(SettingsManager& settings) : m_settings(settings) {}

PageID BankPage::handleInput(const EncoderAction action, char key) {
    if (action == EncoderAction::CLOCKWISE) {
        m_selected_index = (m_selected_index + 1) % m_total_banks;
    }
    else if (action == EncoderAction::COUNTER_CLOCKWISE) {
        m_selected_index = (m_selected_index + m_total_banks - 1) % m_total_banks;
    }
    else if (action == EncoderAction::SINGLE_CLICK) {
        DeviceSettings current = m_settings.load();
        current.current_bank = m_selected_index;
        m_settings.save(current);

        return PageID::MEDIA;
    }

    return PageID::BANK_SELECT;
}

String BankPage::truncate_text(String text, int max_len) {
    if (text.length() <= max_len) return text;
    return text.substring(0, max_len - 3) + "...";
}

void BankPage::draw(Adafruit_SSD1306 &display) {
    display.clearDisplay();

    // --- HEADER ---
    display.fillRect(0, 0, 128, 16, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setTextSize(1);
    display.setCursor(22, 4);
    display.print("SELECT PROFILE");

    // --- THE  WHEEL ---
    const int blue_center_y = 40;

    const int wheel_center_x = -40;
    const int wheel_radius = 65;

    const int text_gap = 10;

    display.drawRoundRect(28, 28, 98, 24, 4, SSD1306_WHITE);

    for (int i = 0; i < m_total_banks; i++) {
        const int distance_from_selected = i - m_selected_index;
        const int y_pos = blue_center_y + (distance_from_selected * 24);

        if (y_pos < 20 || y_pos > 70) continue;

        const int y_offset = y_pos - blue_center_y;
        const float x_curve = sqrt(pow(wheel_radius, 2) - pow(y_offset, 2)) + wheel_center_x;
        const int text_x = static_cast<int>(x_curve) + text_gap;

        if (i == m_selected_index) {
            display.setTextColor(SSD1306_WHITE);

            // Wheel Arc
            display.drawCircle(wheel_center_x-1, blue_center_y, wheel_radius, SSD1306_WHITE);

            // Number
            display.setTextSize(1);
            display.setCursor(text_x, y_pos - 3);
            display.print(i + 1);

            // Name
            display.setTextSize(1);
            display.setCursor(text_x + text_gap, y_pos - 3);
            display.print(truncate_text(m_bank_names[i], 12));
        }
        else {
            display.setTextColor(SSD1306_WHITE);
            display.setTextSize(1);

            display.drawCircle(wheel_center_x, blue_center_y, wheel_radius, SSD1306_WHITE);

            display.setCursor(text_x, y_pos - 3);
            display.print(i + 1);

            display.setCursor(text_x + text_gap, y_pos - 3);
            display.print(truncate_text(m_bank_names[i], 12));
        }
    }

    display.display();
}
