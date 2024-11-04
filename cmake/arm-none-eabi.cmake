# Toolchain file for arm-none-eabi (STM32 / Cortex-M targets)
# Usage: cmake -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi.cmake ..

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

# Adjust these paths if arm-none-eabi-gcc is not in PATH
find_program(ARM_GCC     arm-none-eabi-gcc     REQUIRED)
find_program(ARM_GXX     arm-none-eabi-g++     REQUIRED)
find_program(ARM_OBJCOPY arm-none-eabi-objcopy)
find_program(ARM_SIZE    arm-none-eabi-size)

set(CMAKE_C_COMPILER   ${ARM_GCC})
set(CMAKE_CXX_COMPILER ${ARM_GXX})
set(CMAKE_ASM_COMPILER ${ARM_GCC})

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Common compiler flags for Cortex-M4F (STM32F4)
# Override CORTEX_FLAGS in your project for other targets
set(CORTEX_FLAGS "-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard"
    CACHE STRING "Target-specific CPU flags")

set(CMAKE_C_FLAGS_INIT   "${CORTEX_FLAGS}")
set(CMAKE_CXX_FLAGS_INIT "${CORTEX_FLAGS} -fno-exceptions -fno-rtti")

# Linker flags
set(CMAKE_EXE_LINKER_FLAGS_INIT
    "${CORTEX_FLAGS} -Wl,--gc-sections -specs=nosys.specs -specs=nano.specs")

# Disable compiler tests that require running executables
set(CMAKE_C_COMPILER_WORKS   1)
set(CMAKE_CXX_COMPILER_WORKS 1)

# Helper function to generate binary files
function(arm_generate_bin target)
    if(ARM_OBJCOPY)
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${ARM_OBJCOPY} -O ihex  $<TARGET_FILE:${target}> ${target}.hex
            COMMAND ${ARM_OBJCOPY} -O binary $<TARGET_FILE:${target}> ${target}.bin
            COMMENT "Generating HEX and BIN for ${target}"
        )
    endif()
    if(ARM_SIZE)
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${ARM_SIZE} --format=berkeley $<TARGET_FILE:${target}>
            COMMENT "Size report for ${target}"
        )
    endif()
endfunction()
