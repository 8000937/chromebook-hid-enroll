//
// Created by bradleygumm on 9/2/2026.
//

#ifndef FIRMWARE_EXECUTIONSTATUS_H
#define FIRMWARE_EXECUTIONSTATUS_H
#include <cstdint>

enum class ExecutionStatus : std::int8_t {
    INCOMPLETE = 0,
    COMPLETE = 1,
    TIMED_OUT = -1
};
#endif //FIRMWARE_EXECUTIONSTATUS_H
