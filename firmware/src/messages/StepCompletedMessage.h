//
// Created by bradleygumm on 9/1/2026.
//

#ifndef FIRMWARE_STEPCOMPLETEDMESSAGE_H
#define FIRMWARE_STEPCOMPLETEDMESSAGE_H
#include "StatusMessage.h"

struct StepCompletedMessage : StatusMessage {
    unsigned long msSpentOnTask = 0;
    bool isAutomated = false;

    StepCompletedMessage(StatusMessage& message, unsigned long msSpentOnTask, bool isAutomated) {
        this->informationCode = message.informationCode;
        this->statusCode = message.statusCode;
        this->clientId = message.clientId;
        this->configVersion = message.configVersion;
        this->firmwareVersion = message.firmwareVersion;
        this->jobId = message.jobId;
        this->stepNumber = message.stepNumber;
        this->msSpentOnTask = msSpentOnTask;
        this->isAutomated = isAutomated;
    }
};
#endif //FIRMWARE_STEPCOMPLETEDMESSAGE_H
