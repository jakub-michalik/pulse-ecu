#pragma once

#include "uds/transport/can_interface.hpp"

// STM32F1 CAN peripheral driver
// The STM32F1 uses the same bxCAN peripheral as F4 but with different
// register layout in some areas. This driver handles F1-specific quirks.
//
// STM32F1 CAN features:
//   - Single CAN peripheral (bxCAN)
//   - 14 filter banks (shared with CAN2 if present)
//   - TX mailboxes: 3
//   - RX FIFOs: 2, each 3 messages deep

namespace uds {
namespace stm32 {

class Stm32F1Can : public transport::ICanInterface {
public:
    explicit Stm32F1Can(void* hal_can, uint32_t rx_fifo = 0);

    bool send(const transport::CanFrame& frame) override;
    bool receive(transport::CanFrame& frame) override;
    uint32_t get_tick_ms() override;

    void on_rx_irq();
    bool configure_filter(uint32_t id, uint32_t mask, uint8_t filter_bank = 0);

    // STM32F1-specific: enable silent mode for bus monitoring
    void set_silent_mode(bool enable);

private:
    void*    m_hal_can;
    uint32_t m_rx_fifo;

    static constexpr size_t kRxBufSize = 8; // F1 has less RAM
    transport::CanFrame m_rx_buf[kRxBufSize];
    volatile size_t     m_rx_head;
    volatile size_t     m_rx_tail;
    bool                m_silent_mode;
};

} // namespace stm32
} // namespace uds
