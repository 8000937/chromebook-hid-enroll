//
// Created by bradleygumm on 9/1/2026.
//

#ifndef FIRMWARE_STEPCOMPLETEDMESSAGE_H
#define FIRMWARE_STEPCOMPLETEDMESSAGE_H
#include "StatusMessage.h"
struct StepCompletedMessage : StatusMessage {
private:
    StatusMessage message;

public:
    unsigned long msSpentOnTask = 0;
    bool requiresHumanInput = false;


    StepCompletedMessage(const StatusMessage& msg, unsigned long msSpentOnTask, bool requiresHumanInput) : message(msg) {
        this->informationCode = msg.informationCode;
        this->statusCode = msg.statusCode;
        this->clientId = msg.clientId;
        this->configVersion = msg.configVersion;
        this->firmwareVersion = msg.firmwareVersion;
        this->jobId = msg.jobId;
        this->stepNumber = msg.stepNumber;
        this->msSpentOnTask = msSpentOnTask;
        this->requiresHumanInput = requiresHumanInput;
        this->paused = msg.paused;
    }

    JsonDocument toJson() {
        JsonDocument doc = message.toJson();
        doc["msSpentOnTask"] = msSpentOnTask;
        doc["requiresHumanInput"] = requiresHumanInput;
        return doc;
    }
};
#endif //FIRMWARE_STEPCOMPLETEDMESSAGE_H
