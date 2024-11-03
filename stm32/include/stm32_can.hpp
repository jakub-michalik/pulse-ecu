#pragma once

#include "uds/transport/can_interface.hpp"

// STM32 HAL CAN driver implementation
// Include the appropriate STM32 HAL header before including this file:
//   #include "stm32f4xx_hal.h"  // or stm32f1xx_hal.h, etc.

namespace uds {
namespace stm32 {

class Stm32Can : public transport::ICanInterface {
public:
    // hal_can: pointer to STM32 HAL CAN handle (CAN_HandleTypeDef*)
    // rx_fifo: CAN_RX_FIFO0 or CAN_RX_FIFO1
    explicit Stm32Can(void* hal_can, uint32_t rx_fifo = 0);

    bool send(const transport::CanFrame& frame) override;
    bool receive(transport::CanFrame& frame) override;
    uint32_t get_tick_ms() override;

    // Call from HAL_CAN_RxFifo0MsgPendingCallback or similar ISR
    void on_rx_irq();

    // Configure acceptance filter for given CAN ID
    bool configure_filter(uint32_t id, uint32_t mask, bool extended = false);

private:
    void*    m_hal_can;
    uint32_t m_rx_fifo;

    // Simple ring buffer for received frames
    static constexpr size_t kRxBufSize = 16;
    transport::CanFrame m_rx_buf[kRxBufSize];
    volatile size_t     m_rx_head;
    volatile size_t     m_rx_tail;
};

} // namespace stm32
} // namespace uds

// Note: get_tick_ms() wraps around at ~49 days for uint32_t
// Timeout comparisons use >= which handles this correctly
