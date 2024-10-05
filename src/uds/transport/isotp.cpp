#include "uds/transport/isotp.hpp"
#include <cstring>

namespace uds {
namespace transport {

IsoTp::IsoTp(ICanInterface& can, const IsoTpConfig& config)
    : m_can(can)
    , m_cfg(config)
    , m_rx_state(RxState::Idle)
    , m_rx_expected(0)
    , m_rx_received(0)
    , m_rx_sn(0)
    , m_rx_deadline(0)
    , m_data_ready(false)
    , m_tx_state(TxState::Idle)
    , m_tx_data(nullptr)
    , m_tx_total(0)
    , m_tx_offset(0)
    , m_tx_sn(0)
    , m_tx_bs_count(0)
    , m_tx_bs_rem(0)
    , m_tx_st_min(0)
    , m_tx_deadline(0)
{
}

IsoTpResult IsoTp::process_frame(const CanFrame& frame)
{
    if (frame.id != m_cfg.rx_id || frame.dlc == 0)
        return IsoTpResult::Error;

    uint8_t pci_type = (frame.data[0] >> 4) & 0x0F;

    switch (pci_type) {
    case 0x0: return handle_sf(frame);
    case 0x1: return handle_ff(frame);
    case 0x2: return handle_cf(frame);
    case 0x3: return handle_fc(frame);
    default:  return IsoTpResult::Error;
    }
}

IsoTpResult IsoTp::handle_sf(const CanFrame& frame)
{
    uint8_t len = frame.data[0] & 0x0F;

    // len=0 is invalid; also validate against actual DLC
    if (len == 0 || frame.dlc < 1u || len > static_cast<uint8_t>(frame.dlc - 1u))
        return IsoTpResult::Error;

    if (len > MAX_PAYLOAD)
        return IsoTpResult::Overflow;

    m_data_ready  = false;
    m_rx_state    = RxState::Idle;
    m_rx_expected = len;
    m_rx_received = len;

    memcpy(m_rx_buf, &frame.data[1], len);
    m_data_ready = true;

    return IsoTpResult::Ok;
}

IsoTpResult IsoTp::handle_ff(const CanFrame& frame)
{
    if (frame.dlc < 8)
        return IsoTpResult::Error;

    uint16_t len = static_cast<uint16_t>(((frame.data[0] & 0x0F) << 8) | frame.data[1]);

    if (len < 8)
        return IsoTpResult::Error;

    if (len > MAX_PAYLOAD) {
        send_fc(kFC_OVFL, 0, 0);
        return IsoTpResult::Overflow;
    }

    m_data_ready  = false;
    m_rx_expected = len;
    m_rx_received = 6;
    m_rx_sn       = 1;
    m_rx_state    = RxState::Receiving;
    m_rx_deadline = m_can.get_tick_ms() + m_cfg.n_cr_ms;

    memcpy(m_rx_buf, &frame.data[2], 6);

    send_fc(kFC_CTS, m_cfg.block_size, m_cfg.st_min_ms);

    return IsoTpResult::Pending;
}

IsoTpResult IsoTp::handle_cf(const CanFrame& frame)
{
    if (m_rx_state != RxState::Receiving)
        return IsoTpResult::Error;

    uint8_t sn = frame.data[0] & 0x0F;
    if (sn != m_rx_sn)
        return IsoTpResult::WrongSN;

    m_rx_sn = (m_rx_sn + 1) & 0x0F;

    size_t remaining = m_rx_expected - m_rx_received;
    size_t copy_len  = (remaining < 7u) ? remaining : 7u;
    if (frame.dlc > 0 && copy_len > static_cast<size_t>(frame.dlc - 1u))
        copy_len = frame.dlc - 1u;

    if (m_rx_received + copy_len > MAX_PAYLOAD)
        return IsoTpResult::Overflow;

    memcpy(m_rx_buf + m_rx_received, &frame.data[1], copy_len);
    m_rx_received += copy_len;
    m_rx_deadline  = m_can.get_tick_ms() + m_cfg.n_cr_ms;

    if (m_rx_received >= m_rx_expected) {
        m_rx_received = m_rx_expected; // clamp
        m_rx_state    = RxState::Idle;
        m_data_ready  = true;
        return IsoTpResult::Ok;
    }

    return IsoTpResult::Pending;
}

IsoTpResult IsoTp::handle_fc(const CanFrame& frame)
{
    if (m_tx_state != TxState::WaitFC)
        return IsoTpResult::Error;

    if (frame.dlc < 3)
        return IsoTpResult::Error;

    uint8_t fs  = frame.data[0] & 0x0F;
    uint8_t bs  = frame.data[1];
    uint8_t st  = frame.data[2];

    if (fs == kFC_OVFL)
        return IsoTpResult::Overflow;

    if (fs == kFC_WAIT) {
        m_tx_deadline = m_can.get_tick_ms() + m_cfg.n_bs_ms;
        return IsoTpResult::Pending;
    }

    if (fs != kFC_CTS)
        return IsoTpResult::Error;

    m_tx_bs_rem   = bs;
    m_tx_bs_count = 0;
    m_tx_st_min   = (st <= 0x7F) ? st : 1u;
    m_tx_state    = TxState::Sending;

    return send_next_cf();
}

void IsoTp::send_fc(uint8_t fs, uint8_t bs, uint8_t st_min)
{
    CanFrame fc{};
    fc.id     = m_cfg.tx_id;
    fc.dlc    = 3;
    fc.data[0] = static_cast<uint8_t>((kFC << 4) | (fs & 0x0F));
    fc.data[1] = bs;
    fc.data[2] = st_min;
    m_can.send(fc);
}

IsoTpResult IsoTp::send(const uint8_t* data, size_t length)
{
    if (!data || length == 0 || length > MAX_PAYLOAD)
        return IsoTpResult::Error;

    if (length <= 7u) {
        CanFrame sf{};
        sf.id      = m_cfg.tx_id;
        sf.dlc     = static_cast<uint8_t>(length + 1u);
        sf.data[0] = static_cast<uint8_t>(length & 0x0F);
        memcpy(&sf.data[1], data, length);
        return m_can.send(sf) ? IsoTpResult::Ok : IsoTpResult::Error;
    }

    // First frame
    CanFrame ff{};
    ff.id      = m_cfg.tx_id;
    ff.dlc     = 8;
    ff.data[0] = static_cast<uint8_t>(0x10u | ((length >> 8) & 0x0Fu));
    ff.data[1] = static_cast<uint8_t>(length & 0xFFu);
    memcpy(&ff.data[2], data, 6u);

    if (!m_can.send(ff))
        return IsoTpResult::Error;

    m_tx_data     = data;
    m_tx_total    = length;
    m_tx_offset   = 6u;
    m_tx_sn       = 1u;
    m_tx_bs_count = 0;
    m_tx_bs_rem   = 0;
    m_tx_state    = TxState::WaitFC;
    m_tx_deadline = m_can.get_tick_ms() + m_cfg.n_bs_ms;

    return IsoTpResult::Pending;
}

IsoTpResult IsoTp::send_next_cf()
{
    while (m_tx_offset < m_tx_total) {
        if (m_tx_bs_rem != 0 && m_tx_bs_count >= m_tx_bs_rem) {
            m_tx_state    = TxState::WaitFC;
            m_tx_deadline = m_can.get_tick_ms() + m_cfg.n_bs_ms;
            m_tx_bs_count = 0;
            return IsoTpResult::Pending;
        }

        CanFrame cf{};
        cf.id      = m_cfg.tx_id;
        cf.data[0] = static_cast<uint8_t>(0x20u | (m_tx_sn & 0x0Fu));

        size_t remaining = m_tx_total - m_tx_offset;
        size_t copy_len  = (remaining < 7u) ? remaining : 7u;
        memcpy(&cf.data[1], m_tx_data + m_tx_offset, copy_len);
        cf.dlc = static_cast<uint8_t>(copy_len + 1u);

        if (!m_can.send(cf))
            return IsoTpResult::Error;

        m_tx_offset  += copy_len;
        m_tx_sn       = (m_tx_sn + 1u) & 0x0Fu;
        m_tx_bs_count++;
    }

    m_tx_state = TxState::Idle;
    return IsoTpResult::Ok;
}

size_t IsoTp::get_data(uint8_t* buffer, size_t max_len)
{
    if (!m_data_ready || !buffer) return 0;

    size_t n = (m_rx_received < max_len) ? m_rx_received : max_len;
    memcpy(buffer, m_rx_buf, n);
    m_data_ready  = false;
    m_rx_received = 0;
    return n;
}

void IsoTp::update()
{
    uint32_t now = m_can.get_tick_ms();

    if (m_rx_state == RxState::Receiving && m_rx_deadline > 0 && now >= m_rx_deadline) {
        m_rx_state    = RxState::Idle;
        m_rx_received = 0;
        m_rx_expected = 0;
        m_rx_deadline = 0;
    }

    if (m_tx_state == TxState::WaitFC && m_tx_deadline > 0 && now >= m_tx_deadline) {
        m_tx_state = TxState::Idle;
    }
}

} // namespace transport
} // namespace uds

// Guard: MAX_PAYLOAD checked in handle_cf before memcpy (overflow protection)
