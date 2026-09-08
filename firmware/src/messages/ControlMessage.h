//
// Created by bradleygumm on 9/7/2026.
//

#ifndef FIRMWARE_CONTROLMESSAGE_H
#define FIRMWARE_CONTROLMESSAGE_H
#include <ArduinoJson.h>
#include <WString.h>
#include "ControlCode.h"

struct ControlMessage {
    String clientId;
    ControlCode command;

    JsonDocument toJson() const {
        JsonDocument doc;
        doc["clientId"] = clientId;

        return doc;
    }
};
#endif //FIRMWARE_CONTROLMESSAGE_H
