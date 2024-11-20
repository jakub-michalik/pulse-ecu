#pragma once

#include "uds/transport/can_interface.hpp"
#include "hal_interface.hpp"

namespace uds {
namespace hal {

class CanWithTimer : public transport::ICanInterface {
public:
    CanWithTimer(transport::ICanInterface& can, ITimer& timer)
        : m_can(can), m_timer(timer) {}

    bool     send(const transport::CanFrame& f) override { return m_can.send(f); }
    bool     receive(transport::CanFrame& f) override    { return m_can.receive(f); }
    uint32_t get_tick_ms() override { return m_timer.get_tick_ms(); }

private:
    transport::ICanInterface& m_can;
    ITimer&                   m_timer;
};

} // namespace hal
} // namespace uds
