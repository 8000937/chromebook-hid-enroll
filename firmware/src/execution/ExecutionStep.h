//
// Created by bradleygumm on 9/2/2026.
//

#ifndef FIRMWARE_EXECUTIONSTEP_H
#define FIRMWARE_EXECUTIONSTEP_H
#include <WString.h>

#include <utility>

#include "ExecutionStepType.h"

struct ExecutionStep {
    unsigned long stepId;

    ExecutionStepType type;

    // used for the TYPE STRING step type.
    String toType;

    uint8_t* keys;
    uint8_t keyCount;
    bool rawKeys = false;

    unsigned long timeout = 2000l;

    // only used for the step type of DELAY.
    unsigned long delay = 0l;

    // this is going to be false 90% of the time. This is expected to be true for example, in the "kick off" step.
    // The current idea for that would be waiting for a USB reconnect. The start of this event is based off of human
    // input. However, if the USB Reconnect is expected during a powerwash, for example, this would be false.
    // Typing, and basically every other StepType, this should be false.
    bool requiresHumanInput = false;

    public:
        ExecutionStep(const unsigned long stepId, const ExecutionStepType stepType, String toType, uint8_t* keys, const uint8_t keyCount, const bool areKeysRaw,
            const unsigned long delay, const unsigned long timeout, const bool requiresHumanInput) {
            this->stepId = stepId;
            this->type = stepType;
            this->toType = std::move(toType);
            this->keys = keys;
            this->keyCount = keyCount;
            this->rawKeys = areKeysRaw;
            this->timeout = timeout;
            this->requiresHumanInput = requiresHumanInput;
            this->delay = delay;
        }
};

#endif //FIRMWARE_EXECUTIONSTEP_H
