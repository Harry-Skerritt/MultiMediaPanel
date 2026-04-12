//
// Created by Harry Skerritt on 12/04/2026.
//

#ifndef PAGE_H
#define PAGE_H
#pragma once
#include "DisplayManager/DisplayManager.h"
#include "EncoderManager/EncoderManager.h"

enum class PageID { MEDIA, SETTINGS };

class Page {
public:
    virtual ~Page() = default;

    virtual void draw(Adafruit_SSD1306& display) = 0;
    virtual PageID handleInput (EncoderAction action, char key) = 0;
};



#endif //PAGE_H
