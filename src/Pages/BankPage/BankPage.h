//
// Created by Harry Skerritt on 13/04/2026.
//

#ifndef BANKPAGE_H
#define BANKPAGE_H
#pragma once

#include "../Base/Page.h"
#include "../PageManager/PageManager.h"
#include "../../SettingsManager/SettingsManager.h"

class BankPage : public Page {
public:
    explicit BankPage(SettingsManager& settings);
    PageID handleInput(const EncoderAction action, char key) override;
    void draw(Adafruit_SSD1306 &display) override;

private:
    SettingsManager& m_settings;
    int m_selected_index = 0;
    int m_total_banks = 6; // Todo: Get from TOML
    const char* m_bank_names[6] = {"MEDIA", "WORK", "GAMING", "CODING", "MODELLING", "PHOTOGRAPHY"};


    String truncate_text(String text, int max_len);
};

#endif //BANKPAGE_H
