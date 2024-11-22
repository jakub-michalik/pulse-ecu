/**
 * pulse-ecu STM32F4 example
 * Full UDS server with DTC, security access, flash routines
 */

#include "uds/uds_server.hpp"
#include "uds/services/tester_present.hpp"
#include "uds/services/diagnostic_session_control.hpp"
#include "uds/services/ecu_reset.hpp"
#include "uds/services/read_data_by_identifier.hpp"
#include "uds/services/write_data_by_identifier.hpp"
#include "uds/services/security_access.hpp"
#include "uds/services/routine_control.hpp"
#include "uds/services/read_dtc_information.hpp"
#include "uds/services/clear_diagnostic_information.hpp"
#include "uds/services/control_dtc_setting.hpp"
#include "uds/dtc/dtc_manager.hpp"
#include "uds/security/seed_key.hpp"
#include "uds/transport/isotp.hpp"
#include <cstring>

class StubCan : public uds::transport::ICanInterface {
public:
    bool send(const uds::transport::CanFrame& f) override { (void)f; return true; }
    bool receive(uds::transport::CanFrame& f) override { (void)f; return false; }
    uint32_t get_tick_ms() override { return m_tick; }
    void tick() { m_tick++; }
private:
    uint32_t m_tick = 0;
};

static const char kVin[] = "WVWZZZ1JZ3W386752";
static uint8_t    kSwVer[4] = { 0x01, 0x00, 0x05, 0x00 };

static size_t did_vin(uint8_t* b, size_t m, void*) {
    size_t l = sizeof(kVin)-1; if (l>m) return 0; memcpy(b,kVin,l); return l;
}
static size_t did_sw(uint8_t* b, size_t m, void*) {
    if (m<4) return 0; memcpy(b,kSwVer,4); return 4;
}
static bool did_sw_wr(const uint8_t* d, size_t l, void*) {
    if (l!=4) return false; memcpy(kSwVer,d,4); return true;
}

static uds::security::XorAlgorithm g_xor { 0xDEAD1234 };

static void on_reset(uds::ResetType t, void*) { (void)t; }

static bool routine_erase(const uint8_t*, size_t, uint8_t* r, size_t* l, void*) {
    if (r&&l) { r[0]=0x10; *l=1; } return true;
}

int main()
{
    StubCan can;
    uds::transport::IsoTpConfig cfg = uds::transport::kDefaultIsoTpConfig;
    cfg.rx_id = 0x7E0; cfg.tx_id = 0x7E8;
    uds::transport::IsoTp   isotp(can, cfg);
    uds::UdsServer           server(isotp);

    uds::TesterPresentService              tp;
    uds::DiagnosticSessionControlService   dsc;
    uds::EcuResetService                   reset(on_reset);
    uds::ReadDataByIdentifierService       rdbi;
    uds::WriteDataByIdentifierService      wdbi;
    uds::SecurityAccessService             sec;
    uds::RoutineControlService             rc;
    uds::dtc::DtcManager                   dtc_mgr;
    uds::ReadDtcInformationService         read_dtc(dtc_mgr);
    uds::ClearDiagnosticInformationService clear_dtc(dtc_mgr);
    uds::ControlDtcSettingService          ctrl_dtc;

    rdbi.register_did({ 0xF190, false, did_vin, nullptr, nullptr });
    rdbi.register_did({ 0xF189, true,  did_sw,  did_sw_wr, nullptr });
    wdbi.register_did({ 0xF189, true,  did_sw,  did_sw_wr, nullptr });

    uds::SecurityLevel lvl{};
    lvl.level=0x01; lvl.seed_len=4;
    lvl.generate_seed = uds::security::XorAlgorithm::generate_seed;
    lvl.verify_key    = uds::security::XorAlgorithm::verify_key;
    lvl.required_session = uds::SessionType::Extended;
    lvl.ctx = &g_xor;
    sec.register_level(lvl);

    uds::RoutineEntry er{};
    er.id=uds::routine_id::kEraseFlashSector;
    er.start=routine_erase;
    er.required_session=uds::SessionType::Programming;
    rc.register_routine(er);

    dtc_mgr.register_dtc(0x010101);
    dtc_mgr.register_dtc(0x020202, uds::dtc::DtcSeverity::kCheckAtNextHalt);

    server.register_service(&tp);
    server.register_service(&dsc);
    server.register_service(&reset);
    server.register_service(&rdbi);
    server.register_service(&wdbi);
    server.register_service(&sec);
    server.register_service(&rc);
    server.register_service(&read_dtc);
    server.register_service(&clear_dtc);
    server.register_service(&ctrl_dtc);

    while (true) {
        uds::transport::CanFrame f;
        while (can.receive(f)) isotp.process_frame(f);
        sec.update(can.get_tick_ms());
        server.process();
        can.tick();
    }

    return 0;
}
