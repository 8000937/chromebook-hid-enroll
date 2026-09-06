#include "Keyboard.h"
//
// Created by bradleygumm on 9/6/2026.
//
Keyboard::Keyboard(USBHIDKeyboard& _keyboard) : _keyboard(_keyboard) {}

void Keyboard::writeString(const String& stringToWrite) const {
    _keyboard.print(stringToWrite);
}
void Keyboard::holdKeys(const uint8_t* keys, const uint8_t size) const {
    for (uint8_t i = 0; i < size; i++) {
        _keyboard.press(keys[i]);
    }
}
void Keyboard::holdRawKeys(const uint8_t* keys, const uint8_t size) const {
    for (uint8_t i = 0; i < size; i++) {
        _keyboard.pressRaw(keys[i]);
    }
}

void Keyboard::releaseTheseKeys(const uint8_t* keys, const uint8_t size) const {
    for (uint8_t i = 0; i < size; i++) {
        _keyboard.release(keys[i]);
    }
}

void Keyboard::releaseTheseRawKeys(const uint8_t* keys, const uint8_t size) const {
    // _keyboard.releaseRaw();
    for (uint8_t i = 0; i < size; i++) {
        _keyboard.releaseRaw(keys[i]);
    }
}

void Keyboard::releaseAllKeys() const {
    _keyboard.releaseAll();
}
