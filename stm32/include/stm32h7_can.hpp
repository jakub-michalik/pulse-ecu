#pragma once

#include "uds/transport/can_interface.hpp"

// STM32H7 FDCAN peripheral driver
//
// The STM32H7 uses FDCAN (FD-capable CAN) peripheral, which is different
// from the classic bxCAN used in F1/F4. Key differences:
//   - Message RAM based (configurable)
//   - Supports CAN FD (up to 64 byte payload)
//   - Different HAL API: HAL_FDCAN_xxx instead of HAL_CAN_xxx
//   - FDCAN_HandleTypeDef instead of CAN_HandleTypeDef
//
// This driver implements classic CAN mode (8-byte frames) over FDCAN.

namespace uds {
namespace stm32 {

class Stm32H7Can : public transport::ICanInterface {
public:
    explicit Stm32H7Can(void* hal_fdcan, uint32_t rx_fifo = 0);

    bool     send(const transport::CanFrame& frame) override;
    bool     receive(transport::CanFrame& frame) override;
    uint32_t get_tick_ms() override;

    // Call from HAL_FDCAN_RxFifo0Callback
    void on_rx_irq();

    // Configure acceptance filter
    bool configure_filter(uint32_t id, uint32_t mask, bool extended = false);

    // Enable FDCAN FD mode (64-byte payload support)
    // Note: ISO-TP standard frames are still 8 bytes max
    void enable_fd_mode(bool enable) { m_fd_mode = enable; }

private:
    void*    m_hal_fdcan;
    uint32_t m_rx_fifo;
    bool     m_fd_mode;

    static constexpr size_t kRxBufSize = 16;
    transport::CanFrame m_rx_buf[kRxBufSize];
    volatile size_t     m_rx_head;
    volatile size_t     m_rx_tail;
};

} // namespace stm32
} // namespace uds
