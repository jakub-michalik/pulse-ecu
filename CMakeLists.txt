cmake_minimum_required(VERSION 3.16)
project(pulse-ecu
    VERSION 0.9.0
    DESCRIPTION "UDS library for STM32 microcontrollers"
    LANGUAGES CXX
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(PULSE_BUILD_EXAMPLES "Build examples" OFF)
option(PULSE_BUILD_TESTS    "Build tests"    OFF)
option(PULSE_BUILD_STM32    "Include STM32 HAL driver" OFF)

add_library(pulse-ecu STATIC)

target_include_directories(pulse-ecu
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(pulse-ecu PUBLIC cxx_std_14)

if(CMAKE_CROSSCOMPILING OR PULSE_BUILD_STM32)
    target_compile_options(pulse-ecu PRIVATE
        -Wall -Wextra -Wpedantic
        -fno-exceptions
        -fno-rtti
        -ffunction-sections
        -fdata-sections
    )
else()
    if(MSVC)
        target_compile_options(pulse-ecu PRIVATE /W4)
    else()
        target_compile_options(pulse-ecu PRIVATE -Wall -Wextra)
    endif()
endif()

add_subdirectory(src)

if(PULSE_BUILD_STM32)
    add_subdirectory(stm32)
endif()

if(PULSE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(PULSE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Install targets
include(GNUInstallDirs)

install(TARGETS pulse-ecu
    EXPORT pulse-ecu-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT pulse-ecu-targets
    FILE pulse-ecu-config.cmake
    NAMESPACE pulse::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pulse-ecu
)
