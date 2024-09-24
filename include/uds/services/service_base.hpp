#pragma once

#include "uds/uds_types.hpp"
#include <cstdint>
#include <cstddef>

namespace uds {

class UdsSession;

// Base interface for all UDS services
class IService {
public:
    virtual ~IService() = default;

    // Return the service ID this handler processes
    virtual ServiceId service_id() const = 0;

    // Handle incoming request, write response into resp buffer.
    // resp_len in: max response size, out: actual response size written.
    // Returns true if a response should be sent (false suppresses response).
    virtual bool handle(
        const uint8_t* req,
        size_t         req_len,
        uint8_t*       resp,
        size_t&        resp_len,
        UdsSession&    session) = 0;

protected:
    // Write a negative response
    static void write_nrc(uint8_t* resp, size_t& len, ServiceId sid, NrcCode nrc) {
        resp[0] = static_cast<uint8_t>(ServiceId::NegativeResponse);
        resp[1] = static_cast<uint8_t>(sid);
        resp[2] = static_cast<uint8_t>(nrc);
        len = 3;
    }
};

} // namespace uds

// Helper: check if suppress positive response bit is set (bit 7 of subfunc)
inline bool is_suppress_positive_response(uint8_t sub_func_byte) {
    return (sub_func_byte & 0x80) != 0;
}

// Helper: get subfunc value without suppress bit
inline uint8_t get_sub_func(uint8_t sub_func_byte) {
    return sub_func_byte & 0x7F;
}
