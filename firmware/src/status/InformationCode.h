#pragma once
#include <cstdint>

//
// Created by bradleygumm on 8/27/2026.
//
enum class InformationCode : std::int8_t {
    NONE = 0,
    IDENTIFY = 1,
    NO_WIFI = -1
};