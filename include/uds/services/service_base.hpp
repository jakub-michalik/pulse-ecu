#pragma once

#include "uds/uds_types.hpp"
#include <cstdint>
#include <cstddef>

namespace uds {

class UdsSession;

/**
 * Base interface for all UDS service handlers.
 *
 * To implement a new service:
 *   1. Inherit from IService
 *   2. Implement service_id() returning the 1-byte SID
 *   3. Implement handle() to process the request
 *   4. Register with UdsServer::register_service()
 *
 * The handle() function receives the raw request bytes (including SID),
 * and must write the response into resp[]. resp_len is in/out:
 *   - On entry: maximum bytes available in resp[]
 *   - On exit:  actual bytes written to resp[]
 *
 * Return true if a response should be transmitted.
 * Return false to suppress the response (e.g. suppressPosRspMsgIndicationBit).
 */
class IService {
public:
    virtual ~IService() = default;

    virtual ServiceId service_id() const = 0;

    virtual bool handle(
        const uint8_t* req,
        size_t         req_len,
        uint8_t*       resp,
        size_t&        resp_len,
        UdsSession&    session) = 0;

protected:
    /// Write a 3-byte negative response into resp
    static void write_nrc(uint8_t* resp, size_t& len, ServiceId sid, NrcCode nrc) {
        resp[0] = static_cast<uint8_t>(ServiceId::NegativeResponse);
        resp[1] = static_cast<uint8_t>(sid);
        resp[2] = static_cast<uint8_t>(nrc);
        len = 3;
    }

    /// Ensure resp has at least 'required' bytes, write ResponseTooLong NRC if not
    static bool ensure_space(uint8_t* resp, size_t& len, ServiceId sid, size_t required) {
        if (len < required) {
            write_nrc(resp, len, sid, NrcCode::ResponseTooLong);
            return false;
        }
        return true;
    }
};

/// Check if bit 7 of subfunction byte requests suppression of positive response
inline bool is_suppress_positive_response(uint8_t sub_func_byte) {
    return (sub_func_byte & 0x80) != 0;
}

/// Get subfunc value without the suppress bit
inline uint8_t get_sub_func(uint8_t sub_func_byte) {
    return sub_func_byte & 0x7F;
}

} // namespace uds
