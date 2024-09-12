#pragma once

#include "service_base.hpp"
#include <cstdint>
#include <cstddef>

namespace uds {

struct DownloadRequest {
    uint32_t address;
    size_t   length;
    uint8_t  compression;
    uint8_t  encryption;
};

// Callback to validate and begin download
// Return true if download is accepted, fill max_block_len
using DownloadAcceptCallback = bool (*)(const DownloadRequest& req,
                                        size_t& max_block_len,
                                        void*   ctx);

class RequestDownloadService : public IService {
public:
    RequestDownloadService(DownloadAcceptCallback cb, void* ctx = nullptr)
        : m_callback(cb), m_ctx(ctx), m_active(false) {}

    ServiceId service_id() const override { return ServiceId::RequestDownload; }

    bool handle(
        const uint8_t* req,
        size_t         req_len,
        uint8_t*       resp,
        size_t&        resp_len,
        UdsSession&    session) override;

    bool is_active() const { return m_active; }
    void reset()           { m_active = false; }

private:
    DownloadAcceptCallback m_callback;
    void*   m_ctx;
    bool    m_active;
};

} // namespace uds
