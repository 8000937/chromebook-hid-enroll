//
// Created by bradleygumm on 9/4/2026.
//

#include "UsbManager.h"
#include <USB.h>
#include <USBHIDKeyboard.h>
UsbManager* UsbManager::instance = nullptr;


UsbManager::UsbManager(Stream& serial) : serial(serial) {
    instance = this;
}

void UsbManager::usbStatusCallback(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_base == ARDUINO_USB_EVENTS) {
        switch (event_id) {
            case ARDUINO_USB_STARTED_EVENT:
                serial.println("USB PLUGGED IN");
                previouslyConnected = connected;
                connected = true;
                if (connected && previouslyConnected == false) {
                    transition = 1;
                }
                else if (connected == false && previouslyConnected == true) {
                    transition = -1;
                }
                else {
                    transition = 0;
                }
                break;
            case ARDUINO_USB_STOPPED_EVENT:
                serial.println("USB UNPLUGGED");
                previouslyConnected = connected;
                connected = false;
                if (connected && previouslyConnected == false) {
                    transition = 1;
                }
                else if (connected == false && previouslyConnected == true) {
                    transition = -1;
                }
                else {
                    transition = 0;
                }
                break;
            case ARDUINO_USB_SUSPEND_EVENT:
                serial.println("USB SUSPENDED");
                break;
            case ARDUINO_USB_RESUME_EVENT:
                serial.println("USB RESUMED");
                break;
            default:
                break;
        }
    }
}

int8_t UsbManager::lastTransition() const {
    return transition;
}

volatile bool UsbManager::isConnected() const {
    // serial.printf("IsUSBManager::isConnected %d\n", connected);
    return connected;
}

void UsbManager::setup(const USBHIDKeyboard& Keyboard) {
    // USB.onEvent(usbStatusCallback);
    // keyboard.begin();
    // USB.begin();
    // this->keyboard = USBHIDKeyboard();
    // USB.onEvent(usbStatusCallback);
    this->keyboard.begin();
    USB.begin();
    this->keyboard = Keyboard;
}