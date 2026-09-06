//
// Created by bradleygumm on 9/2/2026.
//

#include "ExecutionManager.h"
ExecutionManager::ExecutionManager(USBHIDKeyboard& _keyboard, UsbManager &usbManager, Stream& serial) : usbManager(usbManager), keyboard(Keyboard(_keyboard)), serial(serial) {}


void ExecutionManager::resetState() {
    previousStepId = -1;
    timer = millis();
}
// only ExecutionStatus of COMPLETE or TIMEOUT should stop or alter execution
// COMPLETE means advance to next step.
// TIMEOUT is an error.
ExecutionStatus ExecutionManager::execute(const ExecutionStep &step) {
    if (previousStepId != step.stepId) {
        resetState();
        previousStepId = step.stepId;
    }

    if (ExecutionStepType::DELAY == step.type) {
        serial.printf("Delay step %ld\n", millis());
        if (millis() - timer >= step.delay) {
            timer = millis();
            serial.printf("Delay complete %ld %ld\n", millis(), millis() - timer);
            return ExecutionStatus::COMPLETE;
        }
    }
    else if (ExecutionStepType::HOLD_KEY == step.type) {
        if (usbManager.isConnected()) {
            keyboard.holdKeys(step.keys, step.keyCount);
            timer = millis();
            return ExecutionStatus::COMPLETE;
        }
    }
    else if (ExecutionStepType::HOLD_RAW_KEY == step.type) {
        if (usbManager.isConnected()) {
            keyboard.holdRawKeys(step.keys, step.keyCount);
            timer = millis();
            return ExecutionStatus::COMPLETE;
        }
    }
    else if (ExecutionStepType::RELEASE_KEY == step.type) {
        if (usbManager.isConnected()) {
            keyboard.releaseTheseKeys(step.keys, step.keyCount);
            timer = millis();
            return ExecutionStatus::COMPLETE;
        }
    }
    else if (ExecutionStepType::RELEASE_RAW_KEY == step.type) {
        //TODO check if keys are raw? what do we do if they're not? This can apply to non raw keys too..
        if (usbManager.isConnected()) {
            keyboard.releaseTheseRawKeys(step.keys, step.keyCount);
            timer = millis();
            return ExecutionStatus::COMPLETE;
        }
    }
    else if (ExecutionStepType::RELEASE_ALL_KEYS == step.type) {
        if (usbManager.isConnected()) {
            keyboard.releaseAllKeys();
            timer = millis();
            return ExecutionStatus::COMPLETE;
        }
    }
    else if (ExecutionStepType::TYPE_STRING == step.type) {
        if (usbManager.isConnected()) {
            keyboard.writeString(step.toType);
            timer = millis();
            return ExecutionStatus::COMPLETE;
        }
    }
    else if (ExecutionStepType::WAIT_FOR_USB_CONNECT == step.type) {
        if (usbManager.lastTransition() == 1) {
            timer = millis();
            return ExecutionStatus::COMPLETE;
        }
    }
    else if (ExecutionStepType::WAIT_FOR_USB_DISCONNECT == step.type) {
        if (usbManager.lastTransition() == -1) {
            timer = millis();
            return ExecutionStatus::COMPLETE;
        }
    }

    if (millis() - timer >= step.timeout) {
        timer = millis();
        return ExecutionStatus::TIMED_OUT;
    }
    return ExecutionStatus::INCOMPLETE;
}
