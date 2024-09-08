#pragma once

#include "service_base.hpp"
#include "uds/dtc/dtc_manager.hpp"

namespace uds {

// ReadDTCInformation sub-functions (partial list)
enum class ReadDtcSubFunc : uint8_t {
    ReportNumberOfDtcByStatusMask        = 0x01,
    ReportDtcByStatusMask                = 0x02,
    ReportDtcSnapshotIdentification      = 0x03,
    ReportDtcSnapshotRecordByDtcNumber   = 0x04,
    ReportDtcExtendedDataRecordByDtcNum  = 0x06,
    ReportSupportedDtc                   = 0x0A,
    ReportFirstTestFailedDtc             = 0x0B,
    ReportFirstConfirmedDtc              = 0x0C,
    ReportMostRecentTestFailedDtc        = 0x0D,
    ReportMostRecentConfirmedDtc         = 0x0E,
    ReportDtcWithPermanentStatus         = 0x15,
};

class ReadDtcInformationService : public IService {
public:
    explicit ReadDtcInformationService(dtc::DtcManager& manager)
        : m_dtc_manager(manager) {}

    ServiceId service_id() const override { return ServiceId::ReadDtcInformation; }

    bool handle(
        const uint8_t* req,
        size_t         req_len,
        uint8_t*       resp,
        size_t&        resp_len,
        UdsSession&    session) override;

private:
    void handle_report_by_status(uint8_t mask, uint8_t* resp, size_t& pos, size_t max);
    void handle_report_count(uint8_t mask, uint8_t* resp, size_t& pos);

    dtc::DtcManager& m_dtc_manager;
};

} // namespace uds
