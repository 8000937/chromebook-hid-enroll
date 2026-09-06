//
// Created by bradleygumm on 9/1/2026.
//

#ifndef FIRMWARE_STATUSMESSAGE_H
#define FIRMWARE_STATUSMESSAGE_H
#include <ArduinoJson.h>
#include <WString.h>

struct StatusMessage{
    int8_t statusCode;
    int8_t informationCode;
    int8_t stepNumber;
    int16_t jobId;
    int16_t configVersion;
    String firmwareVersion;
    String clientId;

    JsonDocument toJson() {
        JsonDocument doc;
        doc["statusCode"] = statusCode;
        doc["informationCode"] = informationCode;
        doc["stepNumber"] = stepNumber;
        doc["jobId"] = jobId;
        doc["configVersion"] = configVersion;
        doc["firmwareVersion"] = firmwareVersion;
        doc["clientId"] = clientId;
        return doc;
    }
 };

#endif //FIRMWARE_STATUSMESSAGE_H
