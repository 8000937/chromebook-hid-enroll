//
// Created by bradleygumm on 9/4/2026.
//

#ifndef FIRMWARE_USBMANAGER_H
#define FIRMWARE_USBMANAGER_H
#include <USB.h>
#include <USBHIDKeyboard.h>

class UsbManager {
public:
    UsbManager(Stream& serial);
    void setup(const USBHIDKeyboard& Keyboard);
    volatile bool isConnected() const;
    int8_t lastTransition() const; // 0 no change, 1 connected, -1 disconnect
    void usbStatusCallback(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

private:
    static UsbManager* instance;
    USBHIDKeyboard keyboard;
    volatile bool connected = false;
    bool previouslyConnected = false;
    int8_t transition = 0;
    Stream& serial;

};


#endif //FIRMWARE_USBMANAGER_H
