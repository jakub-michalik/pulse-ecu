#pragma once

#include "service_base.hpp"
#include "uds/dtc/dtc_manager.hpp"

namespace uds {

class ClearDiagnosticInformationService : public IService {
public:
    explicit ClearDiagnosticInformationService(dtc::DtcManager& manager)
        : m_dtc_manager(manager) {}

    ServiceId service_id() const override {
        return ServiceId::ClearDiagnosticInformation;
    }

    bool handle(
        const uint8_t* req,
        size_t         req_len,
        uint8_t*       resp,
        size_t&        resp_len,
        UdsSession&    session) override;

private:
    dtc::DtcManager& m_dtc_manager;
};

} // namespace uds
