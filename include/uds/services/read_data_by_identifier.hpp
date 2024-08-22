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
