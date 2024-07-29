cmake_minimum_required(VERSION 3.16)
project(pulse-ecu
    VERSION 0.1.0
    DESCRIPTION "UDS library for STM32 microcontrollers"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(PULSE_BUILD_EXAMPLES "Build examples" OFF)
option(PULSE_BUILD_TESTS    "Build tests"    OFF)

add_library(pulse-ecu STATIC)

target_include_directories(pulse-ecu
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(pulse-ecu PUBLIC cxx_std_14)

if(CMAKE_CROSSCOMPILING)
    target_compile_options(pulse-ecu PRIVATE
        -Wall -Wextra -Wpedantic
        -fno-exceptions
        -fno-rtti
        -ffunction-sections
        -fdata-sections
    )
endif()

add_subdirectory(src)

if(PULSE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(PULSE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
