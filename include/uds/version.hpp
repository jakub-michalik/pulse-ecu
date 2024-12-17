#pragma once

#define PULSE_ECU_VERSION_MAJOR 1
#define PULSE_ECU_VERSION_MINOR 0
#define PULSE_ECU_VERSION_PATCH 0

#define PULSE_ECU_VERSION_STRING "1.0.0"

// Numeric version for compile-time comparison:
// version = major*10000 + minor*100 + patch
#define PULSE_ECU_VERSION 10000

namespace uds {

struct Version {
    static constexpr int major = PULSE_ECU_VERSION_MAJOR;
    static constexpr int minor = PULSE_ECU_VERSION_MINOR;
    static constexpr int patch = PULSE_ECU_VERSION_PATCH;
    static constexpr const char* string = PULSE_ECU_VERSION_STRING;
};

} // namespace uds

// Include this header to access version information:
// #include "uds/version.hpp"
// const char* ver = uds::Version::string; // "1.0.0"
