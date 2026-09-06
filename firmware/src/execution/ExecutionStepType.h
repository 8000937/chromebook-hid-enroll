//
// Created by bradleygumm on 9/2/2026.
//

#ifndef FIRMWARE_EXECUTIONSTEPTYPE_H
#define FIRMWARE_EXECUTIONSTEPTYPE_H
#include <cstdint>
/* StepType 10 - 19 = Keyboard
 * StepType 20 - 29 = USB
 * Step 0 - 9 = Utils (timing)
 */
enum class ExecutionStepType : std::uint8_t {
    DELAY = 0,

    HOLD_KEY = 10,
    HOLD_RAW_KEY = 11,

    RELEASE_KEY = 12,
    RELEASE_RAW_KEY = 13,

    RELEASE_ALL_KEYS = 14,

    TYPE_STRING = 15,

    WAIT_FOR_USB_CONNECT = 20,
    WAIT_FOR_USB_DISCONNECT = 21,

};
#endif //FIRMWARE_EXECUTIONSTEPTYPE_H
