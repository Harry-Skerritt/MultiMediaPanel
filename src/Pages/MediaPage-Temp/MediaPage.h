//
// Created by Harry Skerritt on 12/04/2026.
//

#ifndef MEDIAPAGE_H
#define MEDIAPAGE_H
#pragma once
#include "LEDManager/LEDManager.h"
#include "Pages/Base/Page.h"
#include "BleKeyboard.h"


class MediaPage : public Page {
public:
    MediaPage(LEDManager& leds, BleKeyboard& keyboard);

    PageID handleInput(EncoderAction action, char key) override;
    void draw(Adafruit_SSD1306 &display) override;

private:
    LEDManager& m_leds;
    BleKeyboard& m_keyboard;

    char m_active_key = '\0';
    int m_volume_estimate = 50;

    // Mapping labels to the 2x4 grid
    const char* m_labels[2][4] = {
        {"PLAY", "NEXT", "PREV", "STOP"},
        {"RAIN", "BRT+", "BRT-", "SLP!"}
    };
};



#endif //MEDIAPAGE_H
