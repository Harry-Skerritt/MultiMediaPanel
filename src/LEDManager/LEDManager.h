//
// Created by Harry Skerritt on 12/04/2026.
//

#ifndef LEDMANAGER_H
#define LEDMANAGER_H
#pragma once

#include <cstdint>
#include <Adafruit_NeoPixel.h>



class LEDManager {

public:
    LEDManager(uint8_t led_pin, uint8_t led_count);
    void initialise(uint16_t max_brightness = 127);
    void update();

    // --- Lights ---
    void fill(uint8_t r, uint8_t g, uint8_t b);
    void setRainbow(bool active);
    void off();


private:
    Adafruit_NeoPixel m_strip;
    uint16_t m_max_brightness = 0;
    uint32_t m_update_time = 20; // 20ms = 50fps

    // -- Rainbow --
    void updateRainbow();
    bool m_rainbow_active = false;
    uint16_t m_hue = 0;


    // Helpers
    void disableEffects();

};



#endif //LEDMANAGER_H
