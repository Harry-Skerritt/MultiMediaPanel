//
// Created by Harry Skerritt on 12/04/2026.
//

#include "EncoderManager.h"

EncoderManager::EncoderManager(AiEsp32RotaryEncoder &encoder)
    : m_encoder(encoder) {}


EncoderAction EncoderManager::update() {
    const uint32_t now = millis();
    const bool is_pressed = m_encoder.isEncoderButtonDown();

    // --- Rotation ---
    const int current_pos = m_encoder.readEncoder();
    if (current_pos != m_last_pos) {
        EncoderAction rotation = (current_pos > m_last_pos) ? EncoderAction::CLOCKWISE : EncoderAction::COUNTER_CLOCKWISE;
        m_last_pos = current_pos;
        return rotation;
    }

    // --- Button Press ---
    if (is_pressed && !m_was_pressed) {
        m_btn_last_change = now;
        m_was_pressed = true;
        m_is_holding_notified = false;

        if (m_last_click_time != 0 && (now - m_last_click_time < m_double_click_window)) {
            m_is_double_tap_pending = true;
        }
        return EncoderAction::NONE;
    }

    // --- Button Release ---
    if (!is_pressed && m_was_pressed) {
        const uint32_t press_duration = now - m_btn_last_change;
        m_was_pressed = false;

        if (press_duration < m_hold_threshold) {
            if (m_is_double_tap_pending) {
                m_is_double_tap_pending = false;
                m_last_click_time = 0;
                return EncoderAction::DOUBLE_CLICK;
            }
            m_last_click_time = now;
        }

        m_is_double_tap_pending = false;
        return EncoderAction::NONE;
    }

    // --- Hold Logic ---
    if (is_pressed && !m_is_holding_notified) {
        if (now - m_btn_last_change > m_hold_threshold) {
            m_is_holding_notified = true;
            m_last_click_time = 0;

            if (m_is_double_tap_pending) {
                m_is_double_tap_pending = false;
                return EncoderAction::DOUBLE_CLICK_HOLD;
            }
            return EncoderAction::CLICK_HOLD;
        }
    }

    // --- Single Click Timout ---
    if (m_last_click_time != 0 && (now - m_last_click_time > m_double_click_window)) {
        m_last_click_time = 0;

        if (!is_pressed) {
            return EncoderAction::SINGLE_CLICK;
        }
    }

    return EncoderAction::NONE;
}