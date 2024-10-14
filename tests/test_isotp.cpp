/**
 * ISO-TP unit tests
 * Simple test runner without external framework
 */

#include "uds/transport/isotp.hpp"
#include <cstdio>
#include <cstring>
#include <cassert>

using namespace uds::transport;

// ---------------------------------------------------------------------------
// Test helper - loopback CAN driver
// ---------------------------------------------------------------------------
class TestCan : public ICanInterface {
public:
    CanFrame sent[32];
    size_t   sent_count = 0;
    CanFrame rx_queue[32];
    size_t   rx_head = 0, rx_tail = 0;
    uint32_t tick = 0;

    bool send(const CanFrame& f) override {
        if (sent_count < 32) sent[sent_count++] = f;
        return true;
    }

    bool receive(CanFrame& f) override {
        if (rx_head == rx_tail) return false;
        f = rx_queue[rx_tail];
        rx_tail = (rx_tail + 1) % 32;
        return true;
    }

    uint32_t get_tick_ms() override { return tick; }

    void inject(const CanFrame& f) {
        rx_queue[rx_head] = f;
        rx_head = (rx_head + 1) % 32;
    }
};

// ---------------------------------------------------------------------------
// Tests
// ---------------------------------------------------------------------------
static int pass_count = 0;
static int fail_count = 0;

#define TEST(cond) do { \
    if (cond) { pass_count++; } \
    else { fail_count++; printf("FAIL: %s:%d: %s\n", __FILE__, __LINE__, #cond); } \
} while(0)

void test_single_frame_rx()
{
    TestCan  can;
    IsoTpConfig cfg = kDefaultIsoTpConfig;
    cfg.rx_id = 0x7E0;
    cfg.tx_id = 0x7E8;
    IsoTp tp(can, cfg);

    CanFrame sf{};
    sf.id      = 0x7E0;
    sf.dlc     = 5;
    sf.data[0] = 0x04;  // SF, len=4
    sf.data[1] = 0x22;
    sf.data[2] = 0xF1;
    sf.data[3] = 0x90;
    sf.data[4] = 0x00;

    IsoTpResult r = tp.process_frame(sf);
    TEST(r == IsoTpResult::Ok);
    TEST(tp.data_available());

    uint8_t buf[64];
    size_t len = tp.get_data(buf, sizeof(buf));
    TEST(len == 4);
    TEST(buf[0] == 0x22);
    TEST(buf[1] == 0xF1);
    TEST(buf[2] == 0x90);
}

void test_single_frame_tx()
{
    TestCan  can;
    IsoTpConfig cfg = kDefaultIsoTpConfig;
    cfg.rx_id = 0x7E8;
    cfg.tx_id = 0x7E0;
    IsoTp tp(can, cfg);

    uint8_t data[] = { 0x62, 0xF1, 0x90, 0x41, 0x42, 0x43 };
    IsoTpResult r = tp.send(data, sizeof(data));
    TEST(r == IsoTpResult::Ok);
    TEST(can.sent_count == 1);
    TEST(can.sent[0].data[0] == 0x06);  // SF, len=6
    TEST(can.sent[0].data[1] == 0x62);
}

void test_sf_wrong_id()
{
    TestCan  can;
    IsoTpConfig cfg = kDefaultIsoTpConfig;
    cfg.rx_id = 0x7E0;
    IsoTp tp(can, cfg);

    CanFrame sf{};
    sf.id      = 0x7E8;  // wrong ID
    sf.dlc     = 2;
    sf.data[0] = 0x01;

    IsoTpResult r = tp.process_frame(sf);
    TEST(r == IsoTpResult::Error);
}

void test_sf_invalid_length()
{
    TestCan  can;
    IsoTpConfig cfg = kDefaultIsoTpConfig;
    cfg.rx_id = 0x7E0;
    IsoTp tp(can, cfg);

    CanFrame sf{};
    sf.id      = 0x7E0;
    sf.dlc     = 1;
    sf.data[0] = 0x00;  // SF len=0 is invalid

    IsoTpResult r = tp.process_frame(sf);
    TEST(r == IsoTpResult::Error);
}

int main()
{
    printf("Running ISO-TP tests...\n");
    test_single_frame_rx();
    test_single_frame_tx();
    test_sf_wrong_id();
    test_sf_invalid_length();

    printf("Results: %d passed, %d failed\n", pass_count, fail_count);
    return fail_count > 0 ? 1 : 0;
}
