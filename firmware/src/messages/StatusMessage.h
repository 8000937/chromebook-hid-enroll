//
// Created by bradleygumm on 9/1/2026.
//

#ifndef FIRMWARE_STATUSMESSAGE_H
#define FIRMWARE_STATUSMESSAGE_H

#include <WString.h>

struct StatusMessage{
    int8_t statusCode;
    int8_t informationCode;
    int8_t stepNumber;
    int16_t jobId;
    int16_t configVersion;
    String firmwareVersion;
    String clientId;
 };

#endif //FIRMWARE_STATUSMESSAGE_H
