//
// Created by Harry Skerritt on 12/04/2026.
//

#ifndef ENCODERMANAGER_H
#define ENCODERMANAGER_H
#pragma once
#include <AiEsp32RotaryEncoder.h>

enum class EncoderAction {
    NONE,
    CLOCKWISE,
    COUNTER_CLOCKWISE,
    SINGLE_CLICK,
    DOUBLE_CLICK,
    CLICK_HOLD,
    DOUBLE_CLICK_HOLD,
};


class EncoderManager {
public:
    explicit EncoderManager(AiEsp32RotaryEncoder& encoder);

    EncoderAction update();

private:
    AiEsp32RotaryEncoder& m_encoder;
    int m_last_pos = 0;

    uint32_t m_btn_last_change = 0;
    uint32_t m_last_click_time = 0;
    bool m_was_pressed = false;
    bool m_is_holding_notified = false;
    bool m_is_double_tap_pending = false;

    // Configurations
    const uint32_t m_double_click_window = 250; // ms
    const uint32_t m_hold_threshold = 600;      // ms

};



#endif //ENCODERMANAGER_H
