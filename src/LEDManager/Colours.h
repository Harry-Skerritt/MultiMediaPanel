//
// Created by Harry Skerritt on 12/04/2026.
//

#ifndef COLOURS_H
#define COLOURS_H
#include <cstdint>

struct RGBColour {
    uint8_t r;
    uint8_t g;
    uint8_t b;


    explicit RGBColour(const uint8_t red = 0, const uint8_t green = 0, const uint8_t blue = 0)
            : r(red), g(green), b(blue) {}


    static RGBColour fromHex(const uint32_t hex) {
        return RGBColour(
            static_cast<uint8_t>((hex >> 16) & 0xFF),
            static_cast<uint8_t>((hex >> 8) & 0xFF),
            static_cast<uint8_t>(hex & 0xFF)
        );
    }
};


const RGBColour COLOUR_RED(255, 0, 0);
const RGBColour COLOUR_GREEN(0, 255, 0);
const RGBColour COLOUR_BLUE(0, 0, 255);
const RGBColour COLOUR_WHITE(255, 255, 255);
const RGBColour COLOUR_OFF(0, 0, 0);

#endif //COLOURS_H
