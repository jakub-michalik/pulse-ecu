#pragma once

#include "service_base.hpp"
#include <cstdint>

namespace uds {

struct UploadRequest {
    uint32_t address;
    size_t   length;
    uint8_t  compression;
    uint8_t  encryption;
};

using UploadAcceptCallback = bool (*)(const UploadRequest& req,
                                      size_t& max_block_len,
                                      void*   ctx);

class RequestUploadService : public IService {
public:
    RequestUploadService(UploadAcceptCallback cb, void* ctx = nullptr)
        : m_callback(cb), m_ctx(ctx), m_active(false) {}

    ServiceId service_id() const override { return ServiceId::RequestUpload; }

    bool handle(
        const uint8_t* req,
        size_t         req_len,
        uint8_t*       resp,
        size_t&        resp_len,
        UdsSession&    session) override;

    bool is_active() const { return m_active; }
    void reset()           { m_active = false; }

private:
    UploadAcceptCallback m_callback;
    void* m_ctx;
    bool  m_active;
};

} // namespace uds
