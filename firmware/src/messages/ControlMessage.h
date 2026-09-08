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

     ControlMessage(const String &clientId, const ControlCode command) {
         this->clientId = clientId;
         this->command = command;
     }

    JsonDocument toJson() const {
        JsonDocument doc;
        doc["clientId"] = clientId;
        doc["command"] = static_cast<int8_t>(command);
        return doc;
    }

    static ControlMessage fromJson(const JsonDocument& doc) {
        ControlMessage msg( doc["clientId"].as<String>(), static_cast<ControlCode>(doc["command"].as<int8_t>()));
        return msg;
    }
};
#endif //FIRMWARE_CONTROLMESSAGE_H
