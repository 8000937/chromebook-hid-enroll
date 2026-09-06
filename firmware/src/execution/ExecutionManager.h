//
// Created by bradleygumm on 9/2/2026.
//

#ifndef FIRMWARE_EXECUTIONMANAGER_H
#define FIRMWARE_EXECUTIONMANAGER_H
#include <esp32-hal.h>

#include "ExecutionStatus.h"
#include "ExecutionStep.h"
#include "usb/UsbManager.h"
#include "usb/Keyboard.h"

class ExecutionManager {
public:
    ExecutionManager(USBHIDKeyboard& _keyboard, UsbManager& usbManager, Stream& serial);
    ExecutionStatus execute(const ExecutionStep& step);

private:
    void resetState();
    long previousStepId = -1;
    unsigned long timer = millis();

    UsbManager& usbManager;
    const Keyboard keyboard;
    Stream& serial;
};


#endif //FIRMWARE_EXECUTIONMANAGER_H
