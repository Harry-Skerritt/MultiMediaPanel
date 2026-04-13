//
// Created by Harry Skerritt on 12/04/2026.
//

#include "MediaPage.h"


MediaPage::MediaPage(LEDManager& leds, BleKeyboard& keyboard)
    : m_leds(leds), m_keyboard(keyboard)
{}


PageID MediaPage::handleInput(EncoderAction action, char key) {
    m_active_key = key; // Update for drawing logic

    // --- Encoder Logic (Volume) ---
    if (action == EncoderAction::CLOCKWISE) {
        m_keyboard.write(KEY_MEDIA_VOLUME_UP);
        m_leds.onInteraction(RGBColour(0, 255, 100), 100);
    }
    else if (action == EncoderAction::COUNTER_CLOCKWISE) {
        m_keyboard.write(KEY_MEDIA_VOLUME_DOWN);
        m_leds.onInteraction(RGBColour(0, 255, 100), 100);
    }
    else if (action == EncoderAction::SINGLE_CLICK) {
        m_keyboard.write(KEY_MEDIA_MUTE);
        m_leds.onInteraction(COLOUR_RED, 500, true);
    }

    // --- Keypad Logic (2x4 Grid) ---
    if (key) {
        switch(key) {
            case '1': m_keyboard.write(KEY_MEDIA_PLAY_PAUSE); break;
            case '2': m_keyboard.write(KEY_MEDIA_NEXT_TRACK); break;
            case '3': m_keyboard.write(KEY_MEDIA_PREVIOUS_TRACK); break;
            case '4': m_keyboard.write(KEY_MEDIA_STOP); break;
            case '5': m_leds.setRainbow(true); break;
            case '6': /* Future: Brightness + */ break;
            case '7': /* Future: Brightness - */ break;
            case '8': /* Reserved for Sleep in main.cpp */ break;
        }

        // Visual feedback on LEDs
        //if (key != '8') m_leds.onInteraction(COLOUR_WHITE, 200, true);
    }

    return PageID::MEDIA; // Stay on this page
}

void MediaPage::draw(Adafruit_SSD1306 &display) {
    display.clearDisplay();
    display.setTextSize(1);

    // --- Header ---
    display.setCursor(0, 3);
    const StringSumHelper title_str = String(m_page_number) + " - " + m_page_title;
    display.print(title_str);

    display.setCursor(85, 3);
    display.print("BT: OK");
    display.drawLine(0, 13, 128, 13, SSD1306_WHITE);



    // ---Grid Setup ---
    int box_w = 30;
    int box_h = 18;
    int gap = 2;
    int grid_offset = 18;

    // --- Draw Grid ---
    for (int r = 0; r < 2; r++) {
        for (int c = 0; c < 4; c++) {
            const int x = c * (box_w + gap) + 1;
            const int y = r * (box_h + gap) + grid_offset;

            const char key_idx = (r * 4 + c + 1) + '0';

            if (m_active_key == key_idx) {
                display.fillRect(x, y, box_w, box_h, SSD1306_WHITE);
                display.setTextColor(SSD1306_BLACK);
            } else {
                display.drawRect(x, y, box_w, box_h, SSD1306_WHITE);
                display.setTextColor(SSD1306_WHITE);
            }

            display.setCursor(x + 4, y + 5);
            display.print(m_labels[r][c]);
        }
    }

    display.display();
}
