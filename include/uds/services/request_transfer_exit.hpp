#pragma once

#include "service_base.hpp"
#include "request_download.hpp"
#include "transfer_data.hpp"
#include <cstdint>

namespace uds {

using TransferExitCallback = bool (*)(const uint8_t* transfer_req_param,
                                      size_t          param_len,
                                      uint8_t*        resp_param,
                                      size_t*         resp_param_len,
                                      void*           ctx);

class RequestTransferExitService : public IService {
public:
    RequestTransferExitService(
        RequestDownloadService& dl,
        TransferDataService&    td,
        TransferExitCallback    cb  = nullptr,
        void*                   ctx = nullptr)
        : m_dl(dl), m_td(td), m_callback(cb), m_ctx(ctx) {}

    ServiceId service_id() const override { return ServiceId::RequestTransferExit; }

    bool handle(
        const uint8_t* req,
        size_t         req_len,
        uint8_t*       resp,
        size_t&        resp_len,
        UdsSession&    session) override;

private:
    RequestDownloadService& m_dl;
    TransferDataService&    m_td;
    TransferExitCallback    m_callback;
    void*                   m_ctx;
};

} // namespace uds
