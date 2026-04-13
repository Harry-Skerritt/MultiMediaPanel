//
// Created by Harry Skerritt on 12/04/2026.
//

#ifndef PAGEMANAGER_H
#define PAGEMANAGER_H
#include "Adafruit_SSD1306.h"
#include "BleKeyboard.h"
#include "../Base/Page.h"
#include "EncoderManager/EncoderManager.h"
#include "SettingsManager/SettingsManager.h"
#include "LEDManager/LEDManager.h"

// Forward Decs
class MediaPage;
class SettingsPage;
class BankPage;

class PageManager {
public:
    PageManager(LEDManager& leds, BleKeyboard& keyboard, SettingsManager& settings_manager);

    void initialise(const DeviceSettings& settings);

    PageID update(EncoderAction action, char key);
    void switchPage(PageID new_page);
    void draw(Adafruit_SSD1306& display) const;

    PageID getCurrentPageID() const { return m_current_page_id; }

private:
    LEDManager& m_leds;
    Page* m_current_page = nullptr;
    PageID m_current_page_id;

    SettingsPage* m_settings_page;
    MediaPage* m_media_page;
    BankPage* m_bank_page;
};



#endif //PAGEMANAGER_H
