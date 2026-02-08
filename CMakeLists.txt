cmake_minimum_required(VERSION 3.10)

# Toolchain settings - MUST be before project()
if(NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/arm-none-eabi.cmake")
endif()
include(${CMAKE_TOOLCHAIN_FILE})

# Project Name
project(gd32F303_Cmake C ASM)

# CPU Settings
set(CPU_FLAGS "-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard")

# Compiler Flags
set(CMAKE_C_FLAGS "${CPU_FLAGS} -Wall -fdata-sections -ffunction-sections")
set(CMAKE_ASM_FLAGS "${CPU_FLAGS} -x assembler-with-cpp")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os")
endif()

# Linker Flags
set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/ldscripts/gd32f30x_flash.ld)
set(CMAKE_EXE_LINKER_FLAGS "${CPU_FLAGS} -T${LINKER_SCRIPT} -Wl,--gc-sections --specs=nosys.specs --specs=nano.specs -Wl,-Map=${PROJECT_NAME}.map")

# Definitions
add_definitions(-DGD32F30X_HD)
add_definitions(-DUSE_STDPERIPH_DRIVER)
add_definitions(-DGD_ECLIPSE_GCC)

# Include Directories
include_directories(
    inc
    tools/RTT
    Firmware/CMSIS
    Firmware/CMSIS/GD/GD32F30x/Include
    Firmware/GD32F30x_standard_peripheral/Include
)

# Source Files
file(GLOB USER_SOURCES "src/*.c")
file(GLOB RTT_SOURCES "tools/RTT/*.c" "tools/RTT/*.S")
file(GLOB PERIPH_SOURCES "Firmware/GD32F30x_standard_peripheral/Source/*.c")
file(GLOB CMSIS_SOURCES "Firmware/CMSIS/GD/GD32F30x/Source/*.c")
set(STARTUP_SOURCE "gcc_startup/startup_gd32f30x_hd.S")

set(ALL_SOURCES
    ${USER_SOURCES}
    ${RTT_SOURCES}
    ${PERIPH_SOURCES}
    ${CMSIS_SOURCES}
    ${STARTUP_SOURCE}
)

# Executable
add_executable(${PROJECT_NAME}.elf ${ALL_SOURCES})

# Post-build commands
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_SIZE} ${PROJECT_NAME}.elf
    COMMENT "Generating .bin and .hex files, and showing size"
)
