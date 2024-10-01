// STM32 HAL CAN driver implementation for UDS library
// Tested with STM32F4xx series
//
// To use this driver:
//   1. Include stm32f4xx_hal.h (or your target HAL) before including this file
//   2. Call configure_filter() to set up acceptance filter
//   3. Register on_rx_irq() in HAL_CAN_RxFifo0MsgPendingCallback
//   4. Start CAN: HAL_CAN_Start() + HAL_CAN_ActivateNotification()

#include "stm32_can.hpp"

// Uncomment for your target:
// #include "stm32f4xx_hal.h"

namespace uds {
namespace stm32 {

Stm32Can::Stm32Can(void* hal_can, uint32_t rx_fifo)
    : m_hal_can(hal_can)
    , m_rx_fifo(rx_fifo)
    , m_rx_head(0)
    , m_rx_tail(0)
{
}

bool Stm32Can::send(const transport::CanFrame& frame)
{
    if (!m_hal_can) return false;

#ifdef HAL_CAN_MODULE_ENABLED
    CAN_TxHeaderTypeDef tx_header;
    tx_header.StdId              = frame.id & 0x7FFU;
    tx_header.ExtId              = frame.id;
    tx_header.IDE                = frame.is_extended_id ? CAN_ID_EXT : CAN_ID_STD;
    tx_header.RTR                = CAN_RTR_DATA;
    tx_header.DLC                = frame.dlc;
    tx_header.TransmitGlobalTime = DISABLE;

    uint32_t mailbox = 0;
    return HAL_CAN_AddTxMessage(
        static_cast<CAN_HandleTypeDef*>(m_hal_can),
        &tx_header,
        const_cast<uint8_t*>(frame.data),
        &mailbox) == HAL_OK;
#else
    (void)frame;
    return false;
#endif
}

bool Stm32Can::receive(transport::CanFrame& frame)
{
    // Atomically read from ring buffer
    size_t tail = m_rx_tail;
    if (m_rx_head == tail) return false;

    frame    = m_rx_buf[tail];
    m_rx_tail = (tail + 1) % kRxBufSize;
    return true;
}

uint32_t Stm32Can::get_tick_ms()
{
#ifdef HAL_CAN_MODULE_ENABLED
    return HAL_GetTick();
#else
    return 0;
#endif
}

void Stm32Can::on_rx_irq()
{
#ifdef HAL_CAN_MODULE_ENABLED
    CAN_RxHeaderTypeDef rx_header;
    transport::CanFrame frame{};

    if (HAL_CAN_GetRxMessage(
            static_cast<CAN_HandleTypeDef*>(m_hal_can),
            m_rx_fifo, &rx_header, frame.data) == HAL_OK) {

        frame.id             = (rx_header.IDE == CAN_ID_EXT)
                               ? rx_header.ExtId
                               : rx_header.StdId;
        frame.dlc            = static_cast<uint8_t>(rx_header.DLC);
        frame.is_extended_id = (rx_header.IDE == CAN_ID_EXT);

        size_t next = (m_rx_head + 1) % kRxBufSize;
        if (next != m_rx_tail) {
            m_rx_buf[m_rx_head] = frame;
            m_rx_head           = next;
        }
        // else: buffer full, frame dropped
    }
#endif
}

bool Stm32Can::configure_filter(uint32_t id, uint32_t mask, bool extended)
{
#ifdef HAL_CAN_MODULE_ENABLED
    CAN_FilterTypeDef filter;
    filter.FilterBank           = 0;
    filter.FilterMode           = CAN_FILTERMODE_IDMASK;
    filter.FilterScale          = CAN_FILTERSCALE_32BIT;
    filter.FilterFIFOAssignment = (m_rx_fifo == 0) ? CAN_FILTER_FIFO0 : CAN_FILTER_FIFO1;
    filter.FilterActivation     = ENABLE;
    filter.SlaveStartFilterBank = 14;

    if (!extended) {
        filter.FilterIdHigh     = static_cast<uint32_t>((id   & 0x7FF) << 5) & 0xFFFF;
        filter.FilterIdLow      = 0;
        filter.FilterMaskIdHigh = static_cast<uint32_t>((mask & 0x7FF) << 5) & 0xFFFF;
        filter.FilterMaskIdLow  = 0x0002; // IDE bit must be 0 (standard frame)
    } else {
        filter.FilterIdHigh     = static_cast<uint32_t>(id   >> 13) & 0xFFFF;
        filter.FilterIdLow      = static_cast<uint32_t>((id   << 3) | 0x04) & 0xFFFF;
        filter.FilterMaskIdHigh = static_cast<uint32_t>(mask >> 13) & 0xFFFF;
        filter.FilterMaskIdLow  = static_cast<uint32_t>((mask << 3) | 0x04) & 0xFFFF;
    }

    return HAL_CAN_ConfigFilter(
        static_cast<CAN_HandleTypeDef*>(m_hal_can), &filter) == HAL_OK;
#else
    (void)id; (void)mask; (void)extended;
    return true;
#endif
}

} // namespace stm32
} // namespace uds
