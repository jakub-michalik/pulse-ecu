/**
 * pulse-ecu STM32F4 example
 *
 * Demonstrates basic UDS server setup on STM32F4 with HAL CAN.
 * CAN1 is used at 500kbps, physical address 0x7E0 (ECU), 0x7E8 (tester).
 *
 * Build with arm-none-eabi-g++ -std=c++14 -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard
 */

// STM32 HAL - include before UDS headers if using HAL CAN driver
// #include "stm32f4xx_hal.h"

#include "uds/uds_server.hpp"
#include "uds/services/tester_present.hpp"
#include "uds/services/diagnostic_session_control.hpp"
#include "uds/services/ecu_reset.hpp"
#include "uds/services/read_data_by_identifier.hpp"
#include "uds/services/security_access.hpp"
#include "uds/transport/isotp.hpp"
// #include "stm32_can.hpp"

// ---------------------------------------------------------------------------
// Minimal CAN stub (replace with Stm32Can in real application)
// ---------------------------------------------------------------------------
class StubCan : public uds::transport::ICanInterface {
public:
    bool send(const uds::transport::CanFrame& f) override { (void)f; return true; }
    bool receive(uds::transport::CanFrame& f) override { (void)f; return false; }
    uint32_t get_tick_ms() override { return tick_ms; }
    uint32_t tick_ms = 0;
};

// ---------------------------------------------------------------------------
// DID read callbacks
// ---------------------------------------------------------------------------
static uint8_t vin_data[17] = "WVWZZZ1JZ3W386752";

static size_t read_vin(uint8_t* buf, size_t max_len, void* ctx)
{
    (void)ctx;
    size_t len = sizeof(vin_data) - 1;
    if (len > max_len) return 0;
    for (size_t i = 0; i < len; ++i) buf[i] = vin_data[i];
    return len;
}

static uint8_t ecu_sw_version[4] = { 0x01, 0x02, 0x03, 0x04 };

static size_t read_sw_version(uint8_t* buf, size_t max_len, void* ctx)
{
    (void)ctx;
    if (max_len < 4) return 0;
    for (int i = 0; i < 4; ++i) buf[i] = ecu_sw_version[i];
    return 4;
}

// ---------------------------------------------------------------------------
// Security access callbacks
// ---------------------------------------------------------------------------
static size_t generate_seed(uint8_t* buf, size_t max_len, void* ctx)
{
    (void)ctx;
    if (max_len < 4) return 0;
    // Simple non-random seed for demo - in production use RNG hardware
    buf[0] = 0xAB; buf[1] = 0xCD; buf[2] = 0xEF; buf[3] = 0x01;
    return 4;
}

static bool verify_key(const uint8_t* seed, size_t seed_len,
                        const uint8_t* key, size_t key_len, void* ctx)
{
    (void)ctx;
    if (seed_len != 4 || key_len != 4) return false;
    // Simple XOR-based key derivation (not secure - demo only)
    return (key[0] == (seed[0] ^ 0xFF)) &&
           (key[1] == (seed[1] ^ 0xFF)) &&
           (key[2] == (seed[2] ^ 0xFF)) &&
           (key[3] == (seed[3] ^ 0xFF));
}

// ---------------------------------------------------------------------------
// ECU reset callback
// ---------------------------------------------------------------------------
static void on_reset(uds::ResetType type, void* ctx)
{
    (void)ctx;
    // HAL_NVIC_SystemReset(); // uncomment for real hardware
    (void)type;
}

// ---------------------------------------------------------------------------
// Main setup
// ---------------------------------------------------------------------------
int main()
{
    // HAL_Init();
    // SystemClock_Config();
    // MX_CAN1_Init();
    // HAL_CAN_Start(&hcan1);
    // HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);

    StubCan can_driver;

    uds::transport::IsoTpConfig isotp_cfg = uds::transport::kDefaultIsoTpConfig;
    isotp_cfg.rx_id   = 0x7E0;
    isotp_cfg.tx_id   = 0x7E8;

    uds::transport::IsoTp    isotp(can_driver, isotp_cfg);
    uds::UdsServer            server(isotp);

    // Register services
    uds::TesterPresentService          tester_present;
    uds::DiagnosticSessionControlService dsc;
    uds::EcuResetService               ecu_reset(on_reset);
    uds::ReadDataByIdentifierService   rdbi;
    uds::SecurityAccessService         sec_access;

    // Configure DID 0xF190 = VIN
    uds::DataIdentifier vin_did;
    vin_did.did      = 0xF190;
    vin_did.writable = false;
    vin_did.read_fn  = read_vin;
    vin_did.write_fn = nullptr;
    vin_did.ctx      = nullptr;
    rdbi.register_did(vin_did);

    // Configure DID 0xF189 = SW version
    uds::DataIdentifier sw_did;
    sw_did.did      = 0xF189;
    sw_did.writable = false;
    sw_did.read_fn  = read_sw_version;
    sw_did.write_fn = nullptr;
    sw_did.ctx      = nullptr;
    rdbi.register_did(sw_did);

    // Configure security level 0x01 (extended session)
    uds::SecurityLevel lvl;
    lvl.level            = 0x01;
    lvl.seed_len         = 4;
    lvl.generate_seed    = generate_seed;
    lvl.verify_key       = verify_key;
    lvl.required_session = uds::SessionType::Extended;
    lvl.ctx              = nullptr;
    sec_access.register_level(lvl);

    server.register_service(&tester_present);
    server.register_service(&dsc);
    server.register_service(&ecu_reset);
    server.register_service(&rdbi);
    server.register_service(&sec_access);

    // Main loop
    while (true) {
        // Feed received CAN frames to ISO-TP
        uds::transport::CanFrame frame;
        while (can_driver.receive(frame)) {
            isotp.process_frame(frame);
        }

        server.process();

        // HAL_Delay(1);
        can_driver.tick_ms++;
    }

    return 0;
}
