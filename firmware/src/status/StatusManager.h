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
        typedef std::function<void(InformationCode newInfoCode, InformationCode oldInfoCode, StatusCode newStatusCode, StatusCode oldStatusCode)> StatusChangeEventCb;
        StatusManager(StatusLED& statusLed, Stream &serial);
        StatusManager(const Adafruit_NeoPixel& led, Stream &serial);
        void onStatusChange(StatusChangeEventCb statusCodeChangeHook);

        void signalExecutionPaused(bool pause);

        void setStatus(StatusCode statusCode, InformationCode informationCode = InformationCode::NONE);

    private:
        StatusLED statusLed;
        Stream& serial;

        // used to stop subsequent status led updates when the underlying status changes (IE usb unplugged, etc) and the execution is paused.
        volatile bool skipStatusChangeLEDUpdates = false;

        InformationCode currentInformationCode = InformationCode::NONE;
        StatusCode currentStatusCode = StatusCode::START_UP;
        StatusChangeEventCb statusChangeEventCb = nullptr;
    };

#endif //FIRMWARE_STATUSMANAGER_H
