//
// Created by Harry Skerritt on 12/04/2026.
//

#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H
#pragma once

#include <cstdint>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>



class DisplayManager {
public:
    DisplayManager(uint8_t width, uint8_t height);

    bool initialise();
    void update(bool connected, char lastKey, int volume);

    void showMessage(const char* title, const char* msg, uint32_t duration = 1000);
    void setSleep(bool active);


private:
    Adafruit_SSD1306 m_display;
    uint8_t m_width, m_height;

    bool m_is_sleeping = false;

    uint32_t m_message_timer = 0;
    uint32_t m_message_duration = 0;

};



#endif //DISPLAYMANAGER_H
