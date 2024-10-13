enable_testing()

# Simple test runner - no external test framework required
add_executable(test_isotp test_isotp.cpp)
target_link_libraries(test_isotp PRIVATE pulse-ecu)

add_executable(test_session test_session.cpp)
target_link_libraries(test_session PRIVATE pulse-ecu)

add_test(NAME isotp   COMMAND test_isotp)
add_test(NAME session COMMAND test_session)
