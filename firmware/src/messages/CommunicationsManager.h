//
// Created by bradleygumm on 9/1/2026.
//

#ifndef FIRMWARE_COMMUNICATIONSMANAGER_H
#define FIRMWARE_COMMUNICATIONSMANAGER_H
#include <Stream.h>

#include "StatusMessage.h"
#include "StepCompletedMessage.h"


class CommunicationsManager {
    public:
        CommunicationsManager(Stream& serial);
        void sendInformationStatusChange(StatusMessage message);
        void sendStatusChange(StatusMessage message);
        void registerDevice(StatusMessage message);
        void stepCompleted(StepCompletedMessage message);
    private:
        Stream& serial;
};


#endif //FIRMWARE_COMMUNICATIONSMANAGER_H
