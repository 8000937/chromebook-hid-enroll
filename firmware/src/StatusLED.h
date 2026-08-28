//
// Created by bradleygumm on 8/27/2026.
//

#ifndef FIRMWARE_STATUSLED_H
#define FIRMWARE_STATUSLED_H
#include "Adafruit_NeoPixel.h"
#include "Color.h"

    class StatusLED {
    public:
        StatusLED(const Adafruit_NeoPixel& pixel, Stream& serial, const uint8_t& brightness);
        void turnOff();
        void multipleFlash(const Color& color, const uint8_t& count, const uint32_t& speed = 200, const uint32_t& flashSetDelay = 1500);
        void alternateColors(const Color* colors, const uint8_t& colorSize, const uint32_t& speed = 200);
        void solidColor(const Color& color);
        void reset();

    private:
        Adafruit_NeoPixel pixel;
        Stream* serial;
        unsigned long lastTurnOff = millis();


        unsigned long lastFlashUpdate = millis();
        uint8_t flashStep = 0;

        unsigned long lastAlternateTime = millis();
        uint8_t lastAlternateColorIndex = 0;
        uint8_t lastAlternateColorStep = 0;
        bool isOn = false;

        uint8_t brightness;

        unsigned long solidColorTime = 0;

        void showColor(const Color &color);
    };


#endif //FIRMWARE_STATUSLED_H
