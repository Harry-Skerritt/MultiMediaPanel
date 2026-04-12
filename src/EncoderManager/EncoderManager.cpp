//
// Created by Harry Skerritt on 12/04/2026.
//

#include "EncoderManager.h"

EncoderManager::EncoderManager(AiEsp32RotaryEncoder &encoder)
    : m_encoder(encoder) {}


EncoderAction EncoderManager::update() {
    auto action = EncoderAction::NONE;

    // Handle Rotation
    const int current_pos = m_encoder.readEncoder();
    if (current_pos != m_last_pos) {
        action = (current_pos > m_last_pos) ? EncoderAction::CLOCKWISE : EncoderAction::COUNTER_CLOCKWISE;
        m_last_pos = current_pos;
        return action;
    }

    // Handle Button Logic
    const bool is_pressed = m_encoder.isEncoderButtonDown();
    const uint32_t now = millis();

    // Button pressed
    if (is_pressed && !m_was_pressed) {
        m_btn_last_change = now;
        m_was_pressed = true;
        m_is_holding_notified = false;
    }

    // Button released
    else if (!is_pressed && m_was_pressed) {
        const uint32_t press_duration = now - m_btn_last_change;
        m_was_pressed = false;

        if (press_duration < m_hold_threshold) {
            // Check for double click
            if (now - m_last_click_time < m_double_click_window) {
                m_last_click_time = 0; // Reset
                return EncoderAction::DOUBLE_CLICK;
            } else {
                m_last_click_time = now;
            }
        }
    }

    // Handle single click timeout (If no second click came within the window)
    if (m_last_click_time != 0 && (now - m_last_click_time > m_double_click_window) && !is_pressed) {
        m_last_click_time = 0;
        return EncoderAction::SINGLE_CLICK;
    }

    // Handle Hold (While button is still down)
    if (is_pressed && !m_is_holding_notified) {
        if (now - m_btn_last_change > m_hold_threshold) {
            m_is_holding_notified = true;
            return EncoderAction::CLICK_HOLD;
        }
    }

    return action;
}