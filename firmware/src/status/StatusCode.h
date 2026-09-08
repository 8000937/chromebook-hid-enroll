//
// Created by bradleygumm on 8/27/2026.
//
#pragma once
#include <cstdint>

enum class StatusCode : std::int8_t {
    // this could be a timeout
    EXECUTION_ERROR = -10,
    UNEXPECTED_USB_DISCONNECT = -20,
    FIRMWARE_ERROR = -30,

    START_UP = 0,

    WAITING_FOR_JOB = 10,
    WAITING_FOR_CONFIG = 11,
    EXECUTING = 12,
    STEP_COMPLETED = 13,
    PAUSED = 14,
    RESUMED = 15,
    EXECUTION_COMPLETE = 16,

    FIRMWARE_UPDATE_IN_PROGRESS = 30
};
