#pragma once

#include "can_interface.hpp"
#include <cstdint>
#include <cstddef>

namespace uds {
namespace transport {

enum class IsoTpResult {
    Ok,
    Pending,
    Error,
    Timeout,
    Overflow,
    WrongSN,
};

struct IsoTpConfig {
    uint32_t rx_id;       // CAN ID we receive on
    uint32_t tx_id;       // CAN ID we send on
    uint8_t  block_size;  // FC block size (0 = no limit)
    uint8_t  st_min_ms;   // FC separation time min
    uint32_t n_bs_ms;     // N_Bs timeout (ms) - wait for FC
    uint32_t n_cr_ms;     // N_Cr timeout (ms) - wait for CF
};

static constexpr IsoTpConfig kDefaultIsoTpConfig = {
    0x7DF,  // rx_id
    0x7E8,  // tx_id
    0,      // block_size
    0,      // st_min_ms
    1000,   // n_bs_ms
    1000,   // n_cr_ms
};

class IsoTp {
public:
    static constexpr size_t MAX_PAYLOAD = 4096;

    explicit IsoTp(ICanInterface& can, const IsoTpConfig& config = kDefaultIsoTpConfig);

    // Send data - handles segmentation automatically
    IsoTpResult send(const uint8_t* data, size_t length);

    // Feed a received CAN frame into the ISO-TP state machine
    IsoTpResult process_frame(const CanFrame& frame);

    // Returns true when a complete message has been received
    bool data_available() const { return m_data_ready; }

    // Copy received data into buffer, returns number of bytes copied
    size_t get_data(uint8_t* buffer, size_t max_len);

    // Call periodically to handle timeouts
    void update();

private:
    // Frame type nibbles
    static constexpr uint8_t kSF = 0x00;  // Single Frame
    static constexpr uint8_t kFF = 0x10;  // First Frame
    static constexpr uint8_t kCF = 0x20;  // Consecutive Frame
    static constexpr uint8_t kFC = 0x30;  // Flow Control

    // FC flow status
    static constexpr uint8_t kFC_CTS  = 0x00;  // Continue To Send
    static constexpr uint8_t kFC_WAIT = 0x01;  // Wait
    static constexpr uint8_t kFC_OVFL = 0x02;  // Overflow

    enum class RxState { Idle, Receiving };
    enum class TxState { Idle, WaitFC, Sending };

    IsoTpResult handle_sf(const CanFrame& frame);
    IsoTpResult handle_ff(const CanFrame& frame);
    IsoTpResult handle_cf(const CanFrame& frame);
    IsoTpResult handle_fc(const CanFrame& frame);

    void send_fc(uint8_t fs, uint8_t bs, uint8_t st_min);
    IsoTpResult send_next_cf();

    ICanInterface& m_can;
    IsoTpConfig    m_cfg;

    // RX
    RxState  m_rx_state;
    uint8_t  m_rx_buf[MAX_PAYLOAD];
    size_t   m_rx_expected;
    size_t   m_rx_received;
    uint8_t  m_rx_sn;
    uint32_t m_rx_deadline;
    bool     m_data_ready;

    // TX
    TxState        m_tx_state;
    const uint8_t* m_tx_data;
    size_t         m_tx_total;
    size_t         m_tx_offset;
    uint8_t        m_tx_sn;
    uint8_t        m_tx_bs_count;
    uint8_t        m_tx_bs_rem;
    uint32_t       m_tx_st_min;
    uint32_t       m_tx_deadline;
};

} // namespace transport
} // namespace uds

// Helper to convert ST_MIN byte to milliseconds
inline uint32_t st_min_to_ms(uint8_t st_min) {
    if (st_min <= 0x7F) return st_min;          // 0-127ms
    if (st_min >= 0xF1 && st_min <= 0xF9)       // 100-900us -> round to 1ms
        return 1;
    return 1;
}
