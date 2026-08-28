//
// Created by bradleygumm on 8/25/2026.
//

#ifndef UNTITLED2_COLOR_H
#define UNTITLED2_COLOR_H
#include <cstdint>


class Color {
    public:
        Color(uint8_t red, uint8_t green, uint8_t blue);
        void writeToPin(const uint8_t pin, const uint32_t delayInMS = 0) const;
        void breathe(const uint8_t pin, const uint32_t colorChangeDelay = 10, const uint32_t intermissionDelay = 15) const;
        uint8_t getRed() const;
        uint8_t getGreen() const;
        uint8_t getBlue() const;
    private:
        uint8_t red;
        uint8_t green;
        uint8_t blue;


};


#endif //UNTITLED2_COLOR_H
