//
// Created by Harry Skerritt on 12/04/2026.
//

#ifndef LEDMANAGER_H
#define LEDMANAGER_H
#pragma once

#include <cstdint>
#include <Adafruit_NeoPixel.h>

#include "Colours.h"


class LEDManager {

public:
    LEDManager(uint8_t led_pin, uint8_t led_count);
    void initialise(uint16_t max_brightness = 127);
    void update();

    void setBrightness(uint16_t brightness);
    void setEnabled(const bool enabled);

    bool getEnabled() const { return m_enabled; };
    u_int16_t getBrightness() const { return m_max_brightness; }

    void onInteraction(const RGBColour& colour, uint32_t interaction_duration = 1000, bool fade = false);

    // --- Lights ---
    void fill(const RGBColour& colour);
    void setRainbow(bool active);
    void setPulse(bool active, const RGBColour& colour = COLOUR_WHITE, float speed = 0.05);
    void off();


private:
    Adafruit_NeoPixel m_strip;
    uint16_t m_max_brightness = 127;
    uint32_t m_update_time = 20; // 20ms = 50fps
    bool m_enabled = true;

    // -- Static --
    bool m_static_active = false;
    RGBColour m_static_colour;

    // -- Interaction --
    void updateInteraction();
    bool m_interaction_fade = false;
    RGBColour m_interaction_colour;
    uint32_t m_interaction_timer = 0;
    uint32_t m_interaction_duration = 0;

    // -- Rainbow --
    void updateRainbow();
    bool m_rainbow_active = false;
    uint16_t m_hue = 0;

    // -- Pulse --
    void updatePulse();
    bool m_pulse_active = false;
    RGBColour m_pulse_colour;
    float m_pulse_speed = 0.05;
    float m_pulse_step = 0;




    // Helpers
    void disableEffects();
    void fillStrip(const RGBColour& colour);

};



#endif //LEDMANAGER_H
