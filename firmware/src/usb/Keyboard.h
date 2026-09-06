//
// Created by bradleygumm on 9/6/2026.
//

#ifndef FIRMWARE_KEYBOARD_H
#define FIRMWARE_KEYBOARD_H
#include <USBHIDKeyboard.h>


class Keyboard {
public:
    explicit Keyboard(USBHIDKeyboard& _keyboard);
    void writeString(const String& stringToWrite) const;
    void holdKeys(const uint8_t* keys, uint8_t size) const;
    void holdRawKeys(const uint8_t* keys, uint8_t size) const;

    void releaseTheseKeys(const uint8_t* keys, uint8_t size) const;
    void releaseTheseRawKeys(const uint8_t* keys, uint8_t size) const;

    void releaseAllKeys() const;
private:
    USBHIDKeyboard& _keyboard;
};


#endif //FIRMWARE_KEYBOARD_H
