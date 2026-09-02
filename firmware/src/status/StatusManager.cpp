//
// Created by bradleygumm on 8/27/2026.
//

#include "StatusManager.h"

#include "InformationCode.h"
#include "StatusCode.h"

StatusManager::StatusManager(StatusLED &statusLed, Stream& serial) : statusLed(statusLed), serial(serial) {
}

const Color amberColor(215, 80, 50);
const Color blueColor(0, 0, 255);
const Color redColor(255, 0, 0);
const Color greenColor(0, 255, 0);
const Color whiteColor(255, 255, 255);
const Color purpleColor(60, 0, 100);
const Color colors[] = {blueColor, redColor};
const Color pausedColor[] = {greenColor, whiteColor};
const Color firmwareUpdateColor[] = {purpleColor, whiteColor};

void StatusManager::onStatusChange(StatusChangeEventCb statusChangeEventHookCb) {
    this->statusChangeEventCb = std::move(statusChangeEventHookCb);
}

void StatusManager::setStatus(const StatusCode statusCode, const InformationCode informationCode) {
    // handle callback if any
    if (statusCode != currentStatusCode || informationCode != currentInformationCode) {
        serial.println("Status Changed");
        if (this->statusChangeEventCb != nullptr) {
            this->statusChangeEventCb(informationCode, this->currentInformationCode, statusCode, this->currentStatusCode);
        }
        this->currentStatusCode = statusCode;
        this->currentInformationCode = informationCode;
    }
    /* handle led change. I have contemplated putting this in the above if statement,
     * so it only actually does this work if there is something to change. There would be a problem if the led could be
     * changed by something else, but if we keep this class, that shouldn't be possible.
     * Further research needed.
    */
    //handle information code changes and return if its not NONE. This is done so the information code overrides status code
    if (informationCode != InformationCode::NONE) {
        if (informationCode == InformationCode::IDENTIFY) {
            //IDENT
            statusLed.multipleFlash(blueColor, 2, 200, 0);
            // serial.println("IDENTIFY");
        }
        else if (informationCode == InformationCode::NO_WIFI) {
            statusLed.multipleFlash(redColor, 3, 200, 1500);
            // serial.println("NO WIFI");
        }
        return;
    }
    switch (statusCode) {
        //Execution Error
        case StatusCode::EXECUTION_ERROR:
            statusLed.solidColor(redColor);
            // serial.println("EXEC ERROR");
            break;
        //unplanned usb disconnect
        case StatusCode::UNEXPECTED_USB_DISCONNECT:
            statusLed.multipleFlash(redColor, 2, 200, 1500);
            // serial.println("UNPLANNED USB DISCONNECT");
            break;
        // firmware error
        case StatusCode::FIRMWARE_ERROR:
            // serial.println("FIRMWARE ERROR (NO LED)");
            break;
        //Waiting for job
        case StatusCode::WAITING_FOR_JOB:
            statusLed.multipleFlash(amberColor, 2, 350, 0);
            // serial.println("WAITING FOR JOB");
            break;
        //waiting for config
        case StatusCode::WAITING_FOR_CONFIG:
            statusLed.multipleFlash(amberColor, 2, 300, 1500);
            // serial.println("WAITING FOR CONFIG");
            break;
        // executing
        case StatusCode::EXECUTING:
            statusLed.solidColor(greenColor);
            // serial.println("EXECUTING");
            break;
        // step completed
        case StatusCode::STEP_COMPLETED:
            // serial.println("STEP COMPLETED NO LED");
            break;
        // paused
        case StatusCode::PAUSED:
            statusLed.alternateColors(pausedColor, 2, 200);
            // serial.println("PAUSED");
            break;
        // resumed
        case StatusCode::RESUMED:
            // serial.println("RESUMED NO LED");
            break;
        // firmware update in progress
        case StatusCode::FIRMWARE_UPDATE_IN_PROGRESS:
            statusLed.alternateColors(firmwareUpdateColor, 2, 200);
            // serial.println("UPDATE IN PROGRESS");
            break;
        default:
            break;
    }
}
