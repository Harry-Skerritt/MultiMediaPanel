//
// Created by Harry Skerritt on 12/04/2026.
//

#ifndef PAGEMANAGER_H
#define PAGEMANAGER_H
#include "Adafruit_SSD1306.h"
#include "BleKeyboard.h"
#include "Base/Page.h"
#include "EncoderManager/EncoderManager.h"
#include "LEDManager/LEDManager.h"
#include "SettingsPage/SettingsPage.h"

enum class PageID { MEDIA, SETTINGS };

class PageManager {
public:
    PageManager(LEDManager& leds, BleKeyboard& keyboard);

    void switchPage(PageID new_page);
    void update(EncoderAction action, char key) const;
    void draw(Adafruit_SSD1306& display) const;

private:
    Page* m_current_page = nullptr;

    SettingsPage m_settings_page;
};



#endif //PAGEMANAGER_H
