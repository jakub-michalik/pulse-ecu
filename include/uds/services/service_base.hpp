#pragma once

#include "uds/uds_types.hpp"
#include <cstdint>
#include <cstddef>

namespace uds {

class UdsSession;

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
    static void write_nrc(uint8_t* resp, size_t& len, ServiceId sid, NrcCode nrc) {
        resp[0] = static_cast<uint8_t>(ServiceId::NegativeResponse);
        resp[1] = static_cast<uint8_t>(sid);
        resp[2] = static_cast<uint8_t>(nrc);
        len = 3;
    }

    static bool ensure_len(uint8_t* resp, size_t& len,
                           ServiceId sid, size_t required) {
        if (len < required) {
            write_nrc(resp, len, sid, NrcCode::ResponseTooLong);
            return false;
        }
        return true;
    }
};

inline bool is_suppress_positive_response(uint8_t sub_func_byte) {
    return (sub_func_byte & 0x80) != 0;
}

inline uint8_t get_sub_func(uint8_t sub_func_byte) {
    return sub_func_byte & 0x7F;
}

} // namespace uds
