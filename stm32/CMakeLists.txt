# STM32 HAL CAN driver - optional component
# Requires HAL headers to be available in the include path

add_library(pulse-ecu-stm32 STATIC)

target_sources(pulse-ecu-stm32
    PRIVATE
        src/stm32_can.cpp
        src/stm32f1_can.cpp
)

target_include_directories(pulse-ecu-stm32
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(pulse-ecu-stm32 PUBLIC pulse-ecu)

# HAL headers must be provided by the application
# target_include_directories(pulse-ecu-stm32 PUBLIC /path/to/HAL/include)
