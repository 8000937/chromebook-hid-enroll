//
// Created by bradleygumm on 8/25/2026.
//

#include <Arduino.h>
#include "Color.h"

uint8_t red,green,blue;

Color::Color(const uint8_t red, const uint8_t green, const uint8_t blue) {
    this->red = red;
    this->green = green;
    this->blue = blue;
}

void Color::writeToPin(const uint8_t pin, const uint32_t delayInMS) const {
    neopixelWrite(pin, red, green, blue);
    delay(delayInMS);
}

uint8_t Color::getBlue() const {
    return blue;
}

uint8_t Color::getGreen() const {
    return green;
}

uint8_t Color::getRed() const {
    return red;
}

void Color::breathe(const uint8_t pin, const uint32_t colorChangeDelay, const uint32_t intermissionDelay) const {
    const double minValue = 70.0;
    const double redIncrementRate = red / minValue;
    const double greenIncrementRate = green / minValue;
    const double blueIncrementRate = blue / minValue;

    double _red = 0;
    double _green = 0;
    double _blue = 0;

    for (uint8_t i = 0; i < minValue; i++) {
        neopixelWrite(pin,_red+=redIncrementRate,_green+=greenIncrementRate,_blue+=blueIncrementRate);
        delay(colorChangeDelay);
    }
    delay(intermissionDelay);

    for (uint8_t i = 0; i < minValue; i++) {
        neopixelWrite(pin,_red-=redIncrementRate,_green-=greenIncrementRate,_blue-=blueIncrementRate);
        delay(colorChangeDelay);
    }
    delay(intermissionDelay);
}
