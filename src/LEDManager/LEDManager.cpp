//
// Created by Harry Skerritt on 12/04/2026.
//

#include "LEDManager.h"


LEDManager::LEDManager(const uint8_t led_pin, const uint8_t led_count)
    : m_strip(led_count, led_pin, NEO_GRB + NEO_KHZ800) {}

void LEDManager::initialise(const uint16_t max_brightness) {
    m_max_brightness = max_brightness;

    m_strip.begin();
    m_strip.setBrightness(m_max_brightness);
    m_strip.show();
}

void LEDManager::update() {
   if (m_rainbow_active) {
       updateRainbow();
   }
}

// --- Lights ---
void LEDManager::fill(const uint8_t r, const uint8_t g, const uint8_t b) {
    disableEffects();
    for(int i=0; i < m_strip.numPixels(); i++) {
        m_strip.setPixelColor(i, m_strip.Color(r, g, b));
    }
    m_strip.show();
}

void LEDManager::off() {
    m_strip.clear();
    m_strip.show();
}

// -- Rainbow --
void LEDManager::setRainbow(const bool active) {
    m_rainbow_active = active;
}

// Private
void LEDManager::updateRainbow() {
    static uint32_t last_update = 0;

    if (millis() - last_update < m_update_time) {
        return;
    }
    last_update = millis();

    for(int i=0; i < m_strip.numPixels(); i++) {
        uint32_t pixel_hue = m_hue + (i * 65536 / m_strip.numPixels());
        m_strip.setPixelColor(i, m_strip.gamma32(m_strip.ColorHSV(pixel_hue)));
    }

    m_strip.show();
    m_hue += 512;
}


// Helpers - Private
void LEDManager::disableEffects() {
    setRainbow(false);
}









