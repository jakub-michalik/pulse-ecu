#pragma once

#include <cstdint>
#include <cstddef>

namespace uds {

// UDS Service Identifiers (ISO 14229-1)
enum class ServiceId : uint8_t {
    DiagnosticSessionControl       = 0x10,
    EcuReset                       = 0x11,
    ClearDiagnosticInformation     = 0x14,
    ReadDtcInformation             = 0x19,
    ReadDataByIdentifier           = 0x22,
    ReadMemoryByAddress            = 0x23,
    SecurityAccess                 = 0x27,
    CommunicationControl           = 0x28,
    WriteDataByIdentifier          = 0x2E,
    InputOutputControlByIdentifier = 0x2F,
    RoutineControl                 = 0x31,
    RequestDownload                = 0x34,
    RequestUpload                  = 0x35,
    TransferData                   = 0x36,
    RequestTransferExit            = 0x37,
    WriteMemoryByAddress           = 0x3D,
    TesterPresent                  = 0x3E,
    ControlDtcSetting              = 0x85,
    ResponseOnEvent                = 0x86,
    LinkControl                    = 0x87,

    PositiveResponseOffset         = 0x40,
    NegativeResponse               = 0x7F,
};

// Negative Response Codes (ISO 14229-1 Table A-1)
enum class NrcCode : uint8_t {
    GeneralReject                               = 0x10,
    ServiceNotSupported                         = 0x11,
    SubFunctionNotSupported                     = 0x12,
    IncorrectMessageLengthOrInvalidFormat       = 0x13,
    ResponseTooLong                             = 0x14,
    BusyRepeatRequest                           = 0x21,
    ConditionsNotCorrect                        = 0x22,
    RequestSequenceError                        = 0x24,
    NoResponseFromSubnetComponent               = 0x25,
    FailurePreventsExecutionOfRequestedAction   = 0x26,
    RequestOutOfRange                           = 0x31,
    SecurityAccessDenied                        = 0x33,
    InvalidKey                                  = 0x35,
    ExceededNumberOfAttempts                    = 0x36,
    RequiredTimeDelayNotExpired                 = 0x37,
    UploadDownloadNotAccepted                   = 0x70,
    TransferDataSuspended                       = 0x71,
    GeneralProgrammingFailure                   = 0x72,
    WrongBlockSequenceCounter                   = 0x73,
    RequestCorrectlyReceivedResponsePending     = 0x78,
    SubFunctionNotSupportedInActiveSession      = 0x7E,
    ServiceNotSupportedInActiveSession          = 0x7F,
};

// Session Types (ISO 14229-1 Table 40)
enum class SessionType : uint8_t {
    Default     = 0x01,
    Programming = 0x02,
    Extended    = 0x03,
};

// UDS timing parameters (in milliseconds)
struct TimingConfig {
    uint16_t p2_ms;       // Default P2 server response time (default 50ms)
    uint16_t p2_star_ms;  // Enhanced P2* response time (default 5000ms)
    uint16_t s3_ms;       // Session layer S3 timeout (default 5000ms)
};

static constexpr TimingConfig kDefaultTiming = { 50, 5000, 5000 };

// Fixed-size byte buffer for embedded use
template<size_t N>
struct StaticBuffer {
    uint8_t data[N];
    size_t  length;

    StaticBuffer() : length(0) { }

    void clear() { length = 0; }

    bool append(uint8_t b) {
        if (length >= N) return false;
        data[length++] = b;
        return true;
    }

    bool append(const uint8_t* src, size_t len) {
        if (length + len > N) return false;
        for (size_t i = 0; i < len; ++i)
            data[length++] = src[i];
        return true;
    }
};

static constexpr size_t kMaxUdsPayload = 4096;

using UdsBuffer = StaticBuffer<kMaxUdsPayload>;

} // namespace uds

// Service-specific timing constants
namespace timing {
    static constexpr uint16_t kP2DefaultMs   = 50;
    static constexpr uint16_t kP2ProgramMs   = 200;
    static constexpr uint16_t kP2StarDefaultMs = 5000;
    static constexpr uint16_t kS3DefaultMs   = 5000;
}

namespace nrc {
    // Common aliases for NRC codes
    using enum NrcCode;

    // Vendor defined range: 0xF0 - 0xFE
    static constexpr uint8_t kVoltageOutOfRange         = 0xF0;
    static constexpr uint8_t kTemperatureOutOfRange      = 0xF1;
    static constexpr uint8_t kChecksumError              = 0xF2;
}

// Forward declaration helper - include this in headers that need SessionType
// without including the full uds_session.hpp
