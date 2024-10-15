#include "uds/uds_session.hpp"
#include <cstdio>
#include <cassert>

using namespace uds;

static int pass_count = 0;
static int fail_count = 0;

#define TEST(cond) do { \
    if (cond) { pass_count++; } \
    else { fail_count++; printf("FAIL: %s:%d: %s\n", __FILE__, __LINE__, #cond); } \
} while(0)

void test_initial_state()
{
    UdsSession s;
    TEST(s.current() == SessionType::Default);
    TEST(!s.is_timed_out());
}

void test_transition_to_extended()
{
    UdsSession s;
    TEST(s.transition(SessionType::Extended));
    TEST(s.current() == SessionType::Extended);
}

void test_programming_from_default_only()
{
    UdsSession s;
    // Direct to programming - ok from default
    TEST(s.transition(SessionType::Programming));
    TEST(s.current() == SessionType::Programming);
}

void test_programming_from_extended_fail()
{
    UdsSession s;
    s.transition(SessionType::Extended);
    // Programming from extended not allowed
    TEST(!s.transition(SessionType::Programming));
    TEST(s.current() == SessionType::Extended);
}

void test_s3_timeout()
{
    TimingConfig timing = { 50, 5000, 100 }; // s3 = 100ms
    UdsSession s(timing);
    s.transition(SessionType::Extended);
    s.keep_alive(0);

    // Not timed out at t=50
    s.update(50);
    TEST(!s.is_timed_out());

    // Timed out after s3 expires
    s.update(101);
    TEST(s.is_timed_out());
    TEST(s.current() == SessionType::Default);
}

void test_keepalive_resets_timer()
{
    TimingConfig timing = { 50, 5000, 100 };
    UdsSession s(timing);
    s.transition(SessionType::Extended);
    s.keep_alive(0);

    s.update(90);
    s.keep_alive(90);  // refresh timer

    s.update(180);     // would have expired without keepalive
    TEST(!s.is_timed_out());
}

int main()
{
    printf("Running session tests...\n");
    test_initial_state();
    test_transition_to_extended();
    test_programming_from_default_only();
    test_programming_from_extended_fail();
    test_s3_timeout();
    test_keepalive_resets_timer();

    printf("Results: %d passed, %d failed\n", pass_count, fail_count);
    return fail_count > 0 ? 1 : 0;
}
