#include "uds/uds_server.hpp"
#include <cstring>

namespace uds {

UdsServer::UdsServer(transport::IsoTp& transport, const TimingConfig& timing)
    : m_transport(transport)
    , m_session(timing)
    , m_service_count(0)
{
    memset(m_services, 0, sizeof(m_services));
}

bool UdsServer::register_service(IService* service)
{
    if (!service || m_service_count >= kMaxServices)
        return false;

    m_services[m_service_count++] = service;
    return true;
}

void UdsServer::process()
{
    transport::CanFrame frame;
    if (m_transport.data_available()) {
        size_t len = m_transport.get_data(m_req_buf, sizeof(m_req_buf));
        if (len > 0) {
            m_session.keep_alive(0); // TODO: pass real tick
            handle_request(m_req_buf, len);
        }
    }

    m_transport.update();
    m_session.update(0); // TODO: pass real tick
}

void UdsServer::handle_request(const uint8_t* data, size_t len)
{
    if (len == 0) return;

    ServiceId sid = static_cast<ServiceId>(data[0]);

    for (size_t i = 0; i < m_service_count; ++i) {
        if (m_services[i]->service_id() == sid) {
            size_t resp_len = sizeof(m_resp_buf);
            bool send = m_services[i]->handle(data, len, m_resp_buf, resp_len, m_session);
            if (send && resp_len > 0) {
                send_response(m_resp_buf, resp_len);
            }
            return;
        }
    }

    send_nrc(sid, NrcCode::ServiceNotSupported);
}

void UdsServer::send_response(const uint8_t* data, size_t len)
{
    m_transport.send(data, len);
}

void UdsServer::send_nrc(ServiceId sid, NrcCode nrc)
{
    uint8_t resp[3];
    resp[0] = static_cast<uint8_t>(ServiceId::NegativeResponse);
    resp[1] = static_cast<uint8_t>(sid);
    resp[2] = static_cast<uint8_t>(nrc);
    send_response(resp, 3);
}

} // namespace uds
