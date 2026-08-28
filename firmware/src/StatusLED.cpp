//
// Created by bradleygumm on 8/27/2026.
//

#include <Adafruit_NeoPixel.h>
#include "StatusLED.h"

StatusLED::StatusLED(const Adafruit_NeoPixel &pixel, Stream &serial, const uint8_t &brightness) {
    this->pixel = pixel;
    this->brightness = brightness;
    this->serial = &serial;
    this->pixel.setBrightness(brightness);
}

void StatusLED::reset() {
    lastTurnOff = millis();
    lastFlashUpdate = millis();
    flashStep = 0;
    lastAlternateTime = millis();
    lastAlternateColorIndex = 0;
    lastAlternateColorStep = 0;
    isOn = false;
    solidColorTime = 0;

    turnOff();
}

void StatusLED::solidColor(const Color &color) {
    if (solidColorTime == 0 || millis() - solidColorTime > 300) {
        reset();
        showColor(color);
        solidColorTime = millis();
    }
    if (pixel.canShow()) {
        pixel.show();
    }
}

/*
  unsigned long lastAlternateTime = millis();
    uint8_t lastAlternateColorIndex = 0;
uint8_t lastAlternateColorStep = 0;
 */
void StatusLED::alternateColors(const Color *colors, const uint8_t &colorSize, const uint32_t &speed) {
    /*
     * Turn on the first color, wait speed ms, turn off color, wait speed ms, increment colorIndex, repeat
     */

    if (lastAlternateColorIndex == 0) {
        serial->printf("showing color in the %d position\n", lastAlternateColorIndex);
        showColor(colors[lastAlternateColorIndex]);

        isOn = true;

        if (lastAlternateColorIndex == colorSize) {
            lastAlternateColorIndex = 0;
        } else {
            lastAlternateColorIndex++;
        }
        lastAlternateTime = millis();
    } else if (lastAlternateColorIndex > 0 && millis() - lastAlternateTime >= speed) {
        if (isOn) {
            serial->println("turning off");
            turnOff();
            isOn = false;
            lastAlternateTime = millis();
        } else {
            serial->printf("2- showing color in the %d position\n", lastAlternateColorIndex);
            showColor(colors[lastAlternateColorIndex]);
            isOn = true;
            if (lastAlternateColorIndex == colorSize) {
                lastAlternateColorIndex = 0;
            } else {
                lastAlternateColorIndex++;
            }
            lastAlternateTime = millis();
        }
    }
    if (pixel.canShow()) {
        pixel.show();
    }
}

void StatusLED::multipleFlash(const Color &color, const uint8_t &count, const uint32_t &speed,
                              const uint32_t &flashSetDelay) {
    if (flashStep == 0) {
        showColor(color);
        flashStep++;
        lastFlashUpdate = millis();
    } else if (flashStep > 0 && flashStep < (count * 2) && millis() - lastFlashUpdate >= speed) {
        // Odds turn off
        if (flashStep % 2 == 1) {
            turnOff();
        } else {
            showColor(color);
        }
        lastFlashUpdate = millis();
        flashStep++;
    } else if (flashStep == count * 2) {
        turnOff();
        flashStep++;
        lastFlashUpdate = millis();
    } else if (flashStep > count * 2 && millis() - lastFlashUpdate >= flashSetDelay) {
        flashStep = 0;
        lastFlashUpdate = millis();
    }

    if (pixel.canShow()) {
        pixel.show();
    }
}

void StatusLED::turnOff() {
    pixel.clear();
    if (pixel.canShow()) {
        pixel.show();
    }
}

void StatusLED::showColor(const Color &color) {
    pixel.setPixelColor(
        0, Adafruit_NeoPixel::gamma32(Adafruit_NeoPixel::Color(color.getRed(), color.getGreen(), color.getBlue())));
}
