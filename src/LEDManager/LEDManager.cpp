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
    static uint32_t last_update = 0;
    if (millis() - last_update < m_update_time) return;
    last_update = millis();

    updateInteraction();

    if (m_interaction_timer > 0) {
        return;
    }

    if (m_rainbow_active) {
       updateRainbow();
    } else if (m_pulse_active) {
       updatePulse();
    }
}

// --- Lights ---
void LEDManager::fill(const RGBColour &colour) {
    disableEffects();
    m_static_active = true;
    m_static_colour = colour;

    fillStrip(colour);
}

void LEDManager::off() {
    m_strip.clear();
    m_strip.show();
}

// -- Interaction --
void LEDManager::onInteraction(const RGBColour &colour, const uint32_t interaction_duration, const bool fade) {
    m_interaction_colour = colour;
    m_interaction_duration = interaction_duration;
    m_interaction_fade = fade;
    m_interaction_timer = millis();

    if (!m_interaction_fade) {
       fillStrip(colour);
    }
}

void LEDManager::updateInteraction() {
    if (m_interaction_timer == 0) return;

    uint32_t elapsed = millis() - m_interaction_timer;

    if (elapsed > m_interaction_duration) {
        m_interaction_timer = 0;

        if (m_static_active) fillStrip(m_static_colour);
        else if (!m_rainbow_active && !m_pulse_active) off();
        return;
    }

    if (m_interaction_fade) {
        const float half_duration = m_interaction_duration / 2.0f;
        const float intensity = 1.0f - (fabs(static_cast<float>(elapsed) - half_duration) / half_duration);

        const uint8_t r = static_cast<uint8_t>(m_interaction_colour.r * intensity);
        const uint8_t g = static_cast<uint8_t>(m_interaction_colour.g * intensity);
        const uint8_t b = static_cast<uint8_t>(m_interaction_colour.b * intensity);

        fillStrip(RGBColour(r, g, b));
    }
}


// -- Rainbow --
void LEDManager::setRainbow(const bool active) {
    disableEffects();
    m_rainbow_active = active;
}

void LEDManager::updateRainbow() {
    for(int i=0; i < m_strip.numPixels(); i++) {
        uint32_t pixel_hue = m_hue + (i * 65536 / m_strip.numPixels());
        m_strip.setPixelColor(i, m_strip.gamma32(m_strip.ColorHSV(pixel_hue)));
    }

    m_strip.show();
    m_hue += 512;
}


// -- Pulse --
void LEDManager::setPulse(const bool active, const RGBColour& colour, const float speed) {
    disableEffects();
    m_pulse_active = active;
    m_pulse_colour = colour;
    m_pulse_speed = speed;
}

void LEDManager::updatePulse() {
    const float intensity = (sin(m_pulse_step) + 1.0f) / 2.0f;

    const uint8_t r = static_cast<uint8_t>(m_pulse_colour.r * intensity);
    const uint8_t g = static_cast<uint8_t>(m_pulse_colour.g * intensity);
    const uint8_t b = static_cast<uint8_t>(m_pulse_colour.b * intensity);

    for(int i = 0; i < m_strip.numPixels(); i++) {
        m_strip.setPixelColor(i, m_strip.Color(r, g, b));
    }
    m_strip.show();

    m_pulse_step += m_pulse_speed;
    if (m_pulse_step >= 6.283) m_pulse_step = 0;
}


// Helpers - Private
void LEDManager::disableEffects() {
    m_rainbow_active = false;
    m_pulse_active = false;
    m_static_active = false;
}


void LEDManager::fillStrip(const RGBColour &colour) {
    uint32_t raw = m_strip.Color(colour.r, colour.g, colour.b);
    for(int i=0; i < m_strip.numPixels(); i++) {
        m_strip.setPixelColor(i, m_strip.gamma32(raw));
    }

    m_strip.show();
}









