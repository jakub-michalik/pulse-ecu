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

    // Register a service handler (does not take ownership)
    bool register_service(IService* service);

    // Main processing loop - call as often as possible
    void process();

    UdsSession& session() { return m_session; }

private:
    void handle_request(const uint8_t* data, size_t len);
    void send_response(const uint8_t* data, size_t len);
    void send_nrc(ServiceId sid, NrcCode nrc);

    bool check_session_allowed(ServiceId sid);

    transport::IsoTp& m_transport;
    UdsSession        m_session;

    IService* m_services[kMaxServices];
    size_t    m_service_count;

    uint8_t   m_req_buf[transport::IsoTp::MAX_PAYLOAD];
    uint8_t   m_resp_buf[transport::IsoTp::MAX_PAYLOAD];
};

} // namespace uds
