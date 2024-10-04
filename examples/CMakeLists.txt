# Example builds - only for host testing, not cross-compiled by default
if(NOT CMAKE_CROSSCOMPILING)
    add_executable(example_stm32f4 stm32f4/main.cpp)
    target_link_libraries(example_stm32f4 PRIVATE pulse-ecu)
    target_include_directories(example_stm32f4 PRIVATE
        ${CMAKE_SOURCE_DIR}/stm32/include
    )
endif()
