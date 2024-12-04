// STM32H7 FDCAN driver for UDS library
// Tested with STM32H743 / STM32H7B0
//
// CubeMX configuration:
//   - Mode: Classic CAN or FD
//   - Nominal bit timing: 500kbps (recommended for automotive)
//   - Message RAM: allocate at least 4 Rx FIFO + 4 Tx buffer elements
//
// #include "stm32h7xx_hal.h"

#include "stm32h7_can.hpp"

namespace uds {
namespace stm32 {

Stm32H7Can::Stm32H7Can(void* hal_fdcan, uint32_t rx_fifo)
    : m_hal_fdcan(hal_fdcan)
    , m_rx_fifo(rx_fifo)
    , m_fd_mode(false)
    , m_rx_head(0)
    , m_rx_tail(0)
{
}

bool Stm32H7Can::send(const transport::CanFrame& frame)
{
    if (!m_hal_fdcan) return false;

#ifdef HAL_FDCAN_MODULE_ENABLED
    FDCAN_TxHeaderTypeDef hdr;
    hdr.Identifier          = frame.id & (frame.is_extended_id ? 0x1FFFFFFFU : 0x7FFU);
    hdr.IdType              = frame.is_extended_id ? FDCAN_EXTENDED_ID : FDCAN_STANDARD_ID;
    hdr.TxFrameType         = FDCAN_DATA_FRAME;
    hdr.DataLength          = (frame.dlc <= 8)
                              ? static_cast<uint32_t>(frame.dlc << 16)
                              : FDCAN_DLC_BYTES_8;
    hdr.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    hdr.BitRateSwitch       = m_fd_mode ? FDCAN_BRS_ON : FDCAN_BRS_OFF;
    hdr.FDFormat            = m_fd_mode ? FDCAN_FD_CAN : FDCAN_CLASSIC_CAN;
    hdr.TxEventFifoControl  = FDCAN_NO_TX_EVENTS;
    hdr.MessageMarker       = 0;

    return HAL_FDCAN_AddMessageToTxFifoQ(
        static_cast<FDCAN_HandleTypeDef*>(m_hal_fdcan),
        &hdr, const_cast<uint8_t*>(frame.data)) == HAL_OK;
#else
    (void)frame;
    return false;
#endif
}

bool Stm32H7Can::receive(transport::CanFrame& frame)
{
    size_t tail = m_rx_tail;
    if (m_rx_head == tail) return false;
    frame    = m_rx_buf[tail];
    m_rx_tail = (tail + 1) % kRxBufSize;
    return true;
}

uint32_t Stm32H7Can::get_tick_ms()
{
#ifdef HAL_FDCAN_MODULE_ENABLED
    return HAL_GetTick();
#else
    return 0;
#endif
}

void Stm32H7Can::on_rx_irq()
{
#ifdef HAL_FDCAN_MODULE_ENABLED
    FDCAN_RxHeaderTypeDef hdr;
    transport::CanFrame   f{};

    uint32_t fifo = (m_rx_fifo == 0) ? FDCAN_RX_FIFO0 : FDCAN_RX_FIFO1;
    if (HAL_FDCAN_GetRxMessage(
            static_cast<FDCAN_HandleTypeDef*>(m_hal_fdcan),
            fifo, &hdr, f.data) == HAL_OK) {

        f.id             = hdr.Identifier;
        f.dlc            = static_cast<uint8_t>((hdr.DataLength >> 16) & 0xF);
        f.is_extended_id = (hdr.IdType == FDCAN_EXTENDED_ID);

        size_t nxt = (m_rx_head + 1) % kRxBufSize;
        if (nxt != m_rx_tail) { m_rx_buf[m_rx_head] = f; m_rx_head = nxt; }
    }
#endif
}

bool Stm32H7Can::configure_filter(uint32_t id, uint32_t mask, bool extended)
{
#ifdef HAL_FDCAN_MODULE_ENABLED
    FDCAN_FilterTypeDef f;
    f.IdType       = extended ? FDCAN_EXTENDED_ID : FDCAN_STANDARD_ID;
    f.FilterIndex  = 0;
    f.FilterType   = FDCAN_FILTER_MASK;
    f.FilterConfig = (m_rx_fifo == 0) ? FDCAN_FILTER_TO_RXFIFO0 : FDCAN_FILTER_TO_RXFIFO1;
    f.FilterID1    = id;
    f.FilterID2    = mask;

    return HAL_FDCAN_ConfigFilter(
        static_cast<FDCAN_HandleTypeDef*>(m_hal_fdcan), &f) == HAL_OK;
#else
    (void)id; (void)mask; (void)extended;
    return true;
#endif
}

} // namespace stm32
} // namespace uds
