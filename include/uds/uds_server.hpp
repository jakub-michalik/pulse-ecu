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

    void set_tick_provider(uint32_t (*fn)(void*), void* ctx);

    // Register a service handler (does not take ownership)
    // Returns false if SID already registered or max services reached
    bool register_service(IService* service);

    // Find registered service by SID - returns nullptr if not found
    IService* find_service(ServiceId sid) const;

    size_t service_count() const { return m_service_count; }

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
