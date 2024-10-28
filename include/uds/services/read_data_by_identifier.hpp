#pragma once

#include "service_base.hpp"
#include <cstdint>
#include <cstddef>

namespace uds {

static constexpr size_t kMaxDataIdentifiers = 64;

// Data identifier record
struct DataIdentifier {
    uint16_t did;       // 2-byte Data Identifier
    bool     writable;  // If false, write attempts are rejected

    // Read callback: fill buffer with data, return number of bytes written
    // Return 0 to indicate error
    size_t (*read_fn)(uint8_t* buf, size_t max_len, void* ctx);

    // Write callback: validate and store data, return true on success
    bool (*write_fn)(const uint8_t* data, size_t len, void* ctx);

    void* ctx;
};

class ReadDataByIdentifierService : public IService {
public:
    ReadDataByIdentifierService() : m_count(0) {}

    ServiceId service_id() const override {
        return ServiceId::ReadDataByIdentifier;
    }

    // Register a DID - up to kMaxDataIdentifiers entries
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

// Standardized DID ranges (ISO 14229-1 Table C.1)
namespace did {
    static constexpr uint16_t kBootSoftwareId       = 0xF180;
    static constexpr uint16_t kAppSoftwareId        = 0xF181;
    static constexpr uint16_t kAppDataId            = 0xF182;
    static constexpr uint16_t kBootSoftwareVersion  = 0xF183;
    static constexpr uint16_t kAppSoftwareVersion   = 0xF184;
    static constexpr uint16_t kAppDataVersion       = 0xF185;
    static constexpr uint16_t kActiveSession        = 0xF186;
    static constexpr uint16_t kVehicleManufId       = 0xF18A;
    static constexpr uint16_t kSystemSupplierEcuId  = 0xF18C;
    static constexpr uint16_t kVin                  = 0xF190;
    static constexpr uint16_t kVehicleManufEcuHwNum = 0xF191;
    static constexpr uint16_t kEcuSerialNumber      = 0xF18B;
}
