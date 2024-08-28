#pragma once

#include "uds_types.hpp"
#include "uds_session.hpp"
#include "services/service_base.hpp"
#include "transport/isotp.hpp"
#include <cstdint>
#include <cstddef>

namespace uds {

static constexpr size_t kMaxServices = 32;

class UdsServer {
public:
    UdsServer(transport::IsoTp& transport, const TimingConfig& timing = kDefaultTiming);

    // Provide tick source for timing - fn(ctx) returns ms
    void set_tick_provider(uint32_t (*fn)(void*), void* ctx);

    bool register_service(IService* service);

    // Call from main loop or task as often as possible
    void process();

    UdsSession& session() { return m_session; }

private:
    void     handle_request(const uint8_t* data, size_t len);
    void     send_response(const uint8_t* data, size_t len);
    void     send_nrc(ServiceId sid, NrcCode nrc);
    uint32_t get_tick() const;

    transport::IsoTp& m_transport;
    UdsSession        m_session;

    IService* m_services[kMaxServices];
    size_t    m_service_count;

    uint32_t (*m_tick_fn)(void*);
    void*      m_tick_ctx;

    uint8_t m_req_buf[transport::IsoTp::MAX_PAYLOAD];
    uint8_t m_resp_buf[transport::IsoTp::MAX_PAYLOAD];
};

} // namespace uds
