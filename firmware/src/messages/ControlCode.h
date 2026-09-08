//
// Created by bradleygumm on 9/7/2026.
//

#ifndef FIRMWARE_CONTROLCODE_H
#define FIRMWARE_CONTROLCODE_H
#include <cstdint>

enum class ControlCode : std::int8_t {
    DEREGISTER = -1,
    REGISTER = 1,

    PAUSE = -2,
    RESUME = 2,

    SET_JOB = 10,

    RELOAD_CONFIG = 20,

    UPDATE_FIRMWARE = 30,
};
#endif //FIRMWARE_CONTROLCODE_H
