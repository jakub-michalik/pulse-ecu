#include "uds/uds_server.hpp"
#include <cstring>

namespace uds {

UdsServer::UdsServer(transport::IsoTp& transport, const TimingConfig& timing)
    : m_transport(transport)
    , m_session(timing)
    , m_service_count(0)
    , m_tick_fn(nullptr)
    , m_tick_ctx(nullptr)
{
    memset(m_services, 0, sizeof(m_services));
}

void UdsServer::set_tick_provider(uint32_t (*fn)(void*), void* ctx)
{
    m_tick_fn  = fn;
    m_tick_ctx = ctx;
}

uint32_t UdsServer::get_tick() const
{
    return m_tick_fn ? m_tick_fn(m_tick_ctx) : 0;
}

bool UdsServer::register_service(IService* service)
{
    if (!service || m_service_count >= kMaxServices)
        return false;

    // Check for duplicate SID
    for (size_t i = 0; i < m_service_count; ++i) {
        if (m_services[i]->service_id() == service->service_id())
            return false;
    }

    m_services[m_service_count++] = service;
    return true;
}

IService* UdsServer::find_service(ServiceId sid) const
{
    for (size_t i = 0; i < m_service_count; ++i) {
        if (m_services[i]->service_id() == sid)
            return m_services[i];
    }
    return nullptr;
}

void UdsServer::process()
{
    uint32_t tick = get_tick();

    if (m_transport.data_available()) {
        size_t len = m_transport.get_data(m_req_buf, sizeof(m_req_buf));
        if (len > 0) {
            m_session.keep_alive(tick);
            handle_request(m_req_buf, len);
        }
    }

    m_transport.update();
    m_session.update(tick);
}

void UdsServer::handle_request(const uint8_t* data, size_t len)
{
    if (len == 0) return;

    ServiceId sid = static_cast<ServiceId>(data[0]);

    IService* svc = find_service(sid);
    if (!svc) {
        send_nrc(sid, NrcCode::ServiceNotSupported);
        return;
    }

    size_t resp_len = sizeof(m_resp_buf);
    bool do_send = svc->handle(data, len, m_resp_buf, resp_len, m_session);

    if (do_send && resp_len > 0) {
        send_response(m_resp_buf, resp_len);
    }
}

void UdsServer::send_response(const uint8_t* data, size_t len)
{
    m_transport.send(data, len);
}

void UdsServer::send_nrc(ServiceId sid, NrcCode nrc)
{
    uint8_t buf[3] = {
        static_cast<uint8_t>(ServiceId::NegativeResponse),
        static_cast<uint8_t>(sid),
        static_cast<uint8_t>(nrc)
    };
    send_response(buf, 3);
}

} // namespace uds
