#pragma once

#include "service_base.hpp"
#include <cstdint>

namespace uds {

// Callback to handle an incoming data block
// block_seq: block sequence counter (starts at 1)
// data: raw block data
// len: data length
// Return true on success
using TransferDataCallback = bool (*)(uint8_t block_seq,
                                      const uint8_t* data,
                                      size_t len,
                                      void* ctx);

class TransferDataService : public IService {
public:
    TransferDataService(TransferDataCallback cb, void* ctx = nullptr)
        : m_callback(cb), m_ctx(ctx), m_expected_seq(1), m_active(false) {}

    ServiceId service_id() const override { return ServiceId::TransferData; }

    void start() { m_active = true; m_expected_seq = 1; }
    void reset() { m_active = false; m_expected_seq = 1; }

    bool handle(
        const uint8_t* req,
        size_t         req_len,
        uint8_t*       resp,
        size_t&        resp_len,
        UdsSession&    session) override;

private:
    TransferDataCallback m_callback;
    void*   m_ctx;
    uint8_t m_expected_seq;
    bool    m_active;
};

} // namespace uds

// Transfer error callback: called when a block fails
// Return true to retry, false to abort
using TransferErrorCallback = bool (*)(uint8_t block_seq, void* ctx);
