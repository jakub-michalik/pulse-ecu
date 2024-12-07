add_library(pulse-ecu-stm32 STATIC)

target_sources(pulse-ecu-stm32
    PRIVATE
        src/stm32_can.cpp
        src/stm32f1_can.cpp
)

# STM32H7 FDCAN support
option(PULSE_STM32H7 "Include STM32H7 FDCAN driver" OFF)
if(PULSE_STM32H7)
    target_sources(pulse-ecu-stm32 PRIVATE src/stm32h7_can.cpp)
endif()

target_include_directories(pulse-ecu-stm32
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(pulse-ecu-stm32 PUBLIC pulse-ecu)
