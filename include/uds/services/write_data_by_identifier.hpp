#pragma once

#include "service_base.hpp"
#include "read_data_by_identifier.hpp"

namespace uds {

class WriteDataByIdentifierService : public IService {
public:
    WriteDataByIdentifierService() : m_count(0) {}

    ServiceId service_id() const override {
        return ServiceId::WriteDataByIdentifier;
    }

    bool register_did(const DataIdentifier& did_entry);

    bool handle(
        const uint8_t* req,
        size_t         req_len,
        uint8_t*       resp,
        size_t&        resp_len,
        UdsSession&    session) override;

private:
    const DataIdentifier* find_did(uint16_t did) const;

    DataIdentifier m_dids[kMaxDataIdentifiers];
    size_t         m_count;
};

} // namespace uds
