#pragma once

#include "service_base.hpp"

namespace uds {

class TesterPresentService : public IService {
public:
    ServiceId service_id() const override { return ServiceId::TesterPresent; }

    bool handle(
        const uint8_t* req,
        size_t         req_len,
        uint8_t*       resp,
        size_t&        resp_len,
        UdsSession&    session) override;
};

} // namespace uds
