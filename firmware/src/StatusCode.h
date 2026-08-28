//
// Created by bradleygumm on 8/27/2026.
//
#pragma once
#include <cstdint>

enum class StatusCode : std::int8_t {
    EXECUTION_ERROR = -10,
    UNEXPECTED_USB_DISCONNECT = -20,
    FIRMWARE_ERROR = -30,
    WAITING_FOR_JOB = 10,
    WAITING_FOR_CONFIG = 11,
    EXECUTING = 12,
    STEP_COMPLETED = 13,
    PAUSED = 14,
    RESUMED = 15,
    FIRMWARE_UPDATE_IN_PROGRESS = 30
};
