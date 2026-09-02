//
// Created by bradleygumm on 8/27/2026.
//

#ifndef FIRMWARE_STATUSMANAGER_H
#define FIRMWARE_STATUSMANAGER_H
#include <cstdint>

#include "InformationCode.h"
#include "StatusCode.h"
#include "StatusLED.h"

    class StatusManager {
    public:
        // explicit StatusManager(StatusLED& statusLed);

        StatusManager(StatusLED &statusLed, Stream &serial);

        void setStatus(StatusCode statusCode, InformationCode informationCode = InformationCode::NONE) const;

    private:
        StatusLED& statusLed;
        Stream& serial;
    };

#endif //FIRMWARE_STATUSMANAGER_H
