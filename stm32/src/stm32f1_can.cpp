// STM32F1 CAN peripheral implementation
// Uses HAL CAN driver (same API as F4 bxCAN)
//
// Example filter setup for STM32F1:
//   can.configure_filter(0x7E0, 0x7FF, 0);  // filter bank 0
//   can.configure_filter(0x600, 0x700, 1);  // filter bank 1 for UDS broadcast

#include "stm32f1_can.hpp"
// #include "stm32f1xx_hal.h"

namespace uds {
namespace stm32 {

Stm32F1Can::Stm32F1Can(void* hal_can, uint32_t rx_fifo)
    : m_hal_can(hal_can)
    , m_rx_fifo(rx_fifo)
    , m_rx_head(0)
    , m_rx_tail(0)
    , m_silent_mode(false)
{
}

bool Stm32F1Can::send(const transport::CanFrame& frame)
{
    if (!m_hal_can) return false;

#ifdef HAL_CAN_MODULE_ENABLED
    CAN_TxHeaderTypeDef hdr;
    hdr.StdId              = frame.id & 0x7FFU;
    hdr.ExtId              = frame.id;
    hdr.IDE                = frame.is_extended_id ? CAN_ID_EXT : CAN_ID_STD;
    hdr.RTR                = CAN_RTR_DATA;
    hdr.DLC                = frame.dlc;
    hdr.TransmitGlobalTime = DISABLE;

    if (m_silent_mode) return false; // bus monitoring only

    uint32_t mbox = 0;
    return HAL_CAN_AddTxMessage(
        static_cast<CAN_HandleTypeDef*>(m_hal_can),
        &hdr, const_cast<uint8_t*>(frame.data), &mbox) == HAL_OK;
#else
    (void)frame;
    return false;
#endif
}

bool Stm32F1Can::receive(transport::CanFrame& frame)
{
    size_t tail = m_rx_tail;
    if (m_rx_head == tail) return false;
    frame    = m_rx_buf[tail];
    m_rx_tail = (tail + 1) % kRxBufSize;
    return true;
}

uint32_t Stm32F1Can::get_tick_ms()
{
#ifdef HAL_CAN_MODULE_ENABLED
    return HAL_GetTick();
#else
    return 0;
#endif
}

void Stm32F1Can::on_rx_irq()
{
#ifdef HAL_CAN_MODULE_ENABLED
    CAN_RxHeaderTypeDef hdr;
    transport::CanFrame f{};
    if (HAL_CAN_GetRxMessage(
            static_cast<CAN_HandleTypeDef*>(m_hal_can),
            m_rx_fifo, &hdr, f.data) == HAL_OK) {
        f.id             = (hdr.IDE == CAN_ID_EXT) ? hdr.ExtId : hdr.StdId;
        f.dlc            = static_cast<uint8_t>(hdr.DLC);
        f.is_extended_id = (hdr.IDE == CAN_ID_EXT);
        size_t nxt = (m_rx_head + 1) % kRxBufSize;
        if (nxt != m_rx_tail) { m_rx_buf[m_rx_head] = f; m_rx_head = nxt; }
    }
#endif
}

bool Stm32F1Can::configure_filter(uint32_t id, uint32_t mask, uint8_t filter_bank)
{
#ifdef HAL_CAN_MODULE_ENABLED
    CAN_FilterTypeDef f;
    f.FilterBank           = filter_bank;
    f.FilterMode           = CAN_FILTERMODE_IDMASK;
    f.FilterScale          = CAN_FILTERSCALE_32BIT;
    f.FilterFIFOAssignment = (m_rx_fifo == 0) ? CAN_FILTER_FIFO0 : CAN_FILTER_FIFO1;
    f.FilterActivation     = ENABLE;
    f.SlaveStartFilterBank = 14;
    f.FilterIdHigh         = static_cast<uint32_t>((id   & 0x7FF) << 5) & 0xFFFF;
    f.FilterIdLow          = 0;
    f.FilterMaskIdHigh     = static_cast<uint32_t>((mask & 0x7FF) << 5) & 0xFFFF;
    f.FilterMaskIdLow      = 0x0002;
    return HAL_CAN_ConfigFilter(
        static_cast<CAN_HandleTypeDef*>(m_hal_can), &f) == HAL_OK;
#else
    (void)id; (void)mask; (void)filter_bank;
    return true;
#endif
}

void Stm32F1Can::set_silent_mode(bool enable)
{
    m_silent_mode = enable;
}

} // namespace stm32
} // namespace uds
