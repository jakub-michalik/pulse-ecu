#include "uds/dtc/dtc_manager.hpp"
#include <cstring>

namespace uds {
namespace dtc {

DtcManager::DtcManager() : m_count(0)
{
    memset(m_dtcs, 0, sizeof(m_dtcs));
}

int DtcManager::register_dtc(uint32_t code, uint8_t severity)
{
    if (m_count >= kMaxDtcs) return -1;

    // Check for duplicate
    if (find(code)) return -1;

    DtcEntry& e    = m_dtcs[m_count];
    e.dtc_code     = code & 0xFFFFFF;
    e.status       = DtcStatus::kTestNotCompletedSinceLastClear |
                     DtcStatus::kTestNotCompletedThisOpCycle;
    e.severity     = severity;
    e.active       = true;
    e.snapshot_len = 0;
    e.ext_data_len = 0;

    return static_cast<int>(m_count++);
}

void DtcManager::set_test_result(uint32_t code, bool failed)
{
    DtcEntry* e = find(code);
    if (!e || !e->active) return;

    e->status &= ~DtcStatus::kTestNotCompletedThisOpCycle;
    e->status &= ~DtcStatus::kTestNotCompletedSinceLastClear;

    if (failed) {
        e->status |= DtcStatus::kTestFailed;
        e->status |= DtcStatus::kTestFailedThisOpCycle;
        e->status |= DtcStatus::kTestFailedSinceLastClear;
        e->status |= DtcStatus::kPendingDtc;
        e->status |= DtcStatus::kConfirmedDtc;
    } else {
        e->status &= ~DtcStatus::kTestFailed;
        e->status &= ~DtcStatus::kTestFailedThisOpCycle;
        e->status &= ~DtcStatus::kPendingDtc;
    }
}

void DtcManager::clear_all()
{
    for (size_t i = 0; i < m_count; ++i) {
        DtcEntry& e    = m_dtcs[i];
        e.status       = DtcStatus::kTestNotCompletedSinceLastClear |
                         DtcStatus::kTestNotCompletedThisOpCycle;
        e.snapshot_len = 0;
        e.ext_data_len = 0;
    }
}

void DtcManager::clear_group(uint32_t group_mask)
{
    for (size_t i = 0; i < m_count; ++i) {
        if (group_mask == 0xFFFFFF ||
            (m_dtcs[i].dtc_code & group_mask) == (group_mask & 0xFFFFFF)) {
            m_dtcs[i].status = DtcStatus::kTestNotCompletedSinceLastClear |
                               DtcStatus::kTestNotCompletedThisOpCycle;
            m_dtcs[i].snapshot_len = 0;
        }
    }
}

size_t DtcManager::count_by_status(uint8_t status_mask) const
{
    size_t cnt = 0;
    for (size_t i = 0; i < m_count; ++i)
        if (status_mask == 0xFF || (m_dtcs[i].status & status_mask) != 0)
            ++cnt;
    return cnt;
}

void DtcManager::for_each(
    uint8_t status_mask,
    bool (*callback)(const DtcEntry&, void*),
    void* ctx) const
{
    for (size_t i = 0; i < m_count; ++i) {
        if (status_mask == 0xFF || (m_dtcs[i].status & status_mask) != 0) {
            if (!callback(m_dtcs[i], ctx)) break;
        }
    }
}

DtcEntry* DtcManager::find(uint32_t code)
{
    uint32_t c = code & 0xFFFFFF;
    for (size_t i = 0; i < m_count; ++i)
        if (m_dtcs[i].dtc_code == c) return &m_dtcs[i];
    return nullptr;
}

void DtcManager::update_op_cycle()
{
    for (size_t i = 0; i < m_count; ++i) {
        m_dtcs[i].status &= ~DtcStatus::kTestFailedThisOpCycle;
        m_dtcs[i].status |=  DtcStatus::kTestNotCompletedThisOpCycle;
    }
}

bool DtcManager::set_snapshot(uint32_t code, const uint8_t* data, size_t len)
{
    DtcEntry* e = find(code);
    if (!e || !data) return false;

    size_t copy = (len < sizeof(e->snapshot)) ? len : sizeof(e->snapshot);
    memcpy(e->snapshot, data, copy);
    e->snapshot_len = copy;
    return true;
}

} // namespace dtc
} // namespace uds
