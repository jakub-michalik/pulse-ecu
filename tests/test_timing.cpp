#include "uds/uds_session.hpp"
#include "uds/hal/hal_timer.hpp"
#include <cstdio>

using namespace uds;
using namespace uds::hal;

static int pass_count = 0, fail_count = 0;
#define TEST(cond) do { \
    if (cond) pass_count++; \
    else { fail_count++; printf("FAIL: %s:%d\n", __FILE__, __LINE__); } \
} while(0)

struct MockTimer : public ITimer {
    uint32_t tick = 0;
    uint32_t get_tick_ms() override { return tick; }
};

void test_is_elapsed_overflow() {
    TEST(is_elapsed(0xFFFFFF00U, 0x00000063U, 99));
    TEST(!is_elapsed(0xFFFFFF00U, 0x00000062U, 99));
}

void test_soft_timer() {
    MockTimer mt;
    SoftTimer t(mt);
    t.start();
    TEST(!t.is_expired(100));
    mt.tick = 100;
    TEST(t.is_expired(100));
}

void test_soft_timer_overflow() {
    MockTimer mt;
    mt.tick = 0xFFFFFF00U;
    SoftTimer t(mt);
    t.start();
    mt.tick = 0x00000064U;
    TEST(t.is_expired(100));
}

int main() {
    printf("Running timing tests...\n");
    test_is_elapsed_overflow();
    test_soft_timer();
    test_soft_timer_overflow();
    printf("Results: %d passed, %d failed\n", pass_count, fail_count);
    return fail_count > 0 ? 1 : 0;
}
