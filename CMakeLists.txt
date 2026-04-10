cmake_minimum_required(VERSION 3.10)

# 1. 工具链设置
if(NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/arm-none-eabi.cmake")
endif()
include(${CMAKE_TOOLCHAIN_FILE})

# 2. 项目定义
project(Platform_Base_Project C ASM)

# Fixed target selection (single-chip GD32F303)
set(BOARD demo_board CACHE STRING "Board name under boards/")
set(APP demo_flash CACHE STRING "App name under app/")

# 3. GD32F30x 芯片配置
set(CPU_FLAGS "-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard")
add_definitions(-DGD32F30X_HD)
set(MCU_LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/platform/chip/gd32f30x/linker/gd32f30x_flash.ld")

# 芯片 SDK 头文件路径
set(MCU_SPECIFIC_INCLUDES
    platform/chip/gd32f30x/sdk/CMSIS
    platform/chip/gd32f30x/sdk/CMSIS/GD/GD32F30x/Include
    platform/chip/gd32f30x/sdk/GD32F30x_standard_peripheral/Include
    platform/hal/src/gd32f30x
)

# 芯片 SDK 源文件
file(GLOB MCU_SPECIFIC_SOURCES
    "platform/chip/gd32f30x/sdk/CMSIS/GD/GD32F30x/Source/*.c"
    "platform/chip/gd32f30x/sdk/GD32F30x_standard_peripheral/Source/*.c"
    "platform/hal/src/gd32f30x/*.c"
)

# 启动文件
set(MCU_STARTUP_FILE "platform/chip/gd32f30x/startup/startup_gd32f30x_hd.S")

# 5. 全局编译选项
set(CMAKE_C_FLAGS "${CPU_FLAGS} -Wall -fdata-sections -ffunction-sections" CACHE INTERNAL "")
set(CMAKE_ASM_FLAGS "${CPU_FLAGS} -x assembler-with-cpp" CACHE INTERNAL "")
set(CMAKE_EXE_LINKER_FLAGS "${CPU_FLAGS} -T${MCU_LINKER_SCRIPT} -Wl,--gc-sections --specs=nosys.specs --specs=nano.specs -Wl,-Map=${PROJECT_NAME}.map" CACHE INTERNAL "")

# 6. 包含目录
include_directories(
    platform/hal/inc
    platform/utils
    platform/drivers/sfud
    platform/drivers/sfud/inc
    boards/${BOARD}
    boards/${BOARD}/config
    app/${APP}/inc
    ${MCU_SPECIFIC_INCLUDES}
)

# 7. 汇总源码
# L5/L3 通用源码
file(GLOB_RECURSE COMMON_SOURCES "platform/utils/*.c")
file(GLOB_RECURSE DRIVER_SOURCES "platform/drivers/**/*.c")

# L1 MHAL 通用层源码（新增的通用逻辑层）
set(HAL_COMMON_SOURCES
    platform/hal/src/hal_gpio.c
    platform/hal/src/hal_spi.c
)

# 产品特定代码
set(APP_SOURCES
    app/${APP}/src/main.c
)

set(BOARD_SOURCES
    boards/${BOARD}/board.c
)

# 8. 生成可执行文件
add_executable(${PROJECT_NAME}.elf
    ${APP_SOURCES}
    ${BOARD_SOURCES}
    ${COMMON_SOURCES}
    ${DRIVER_SOURCES}
    ${HAL_COMMON_SOURCES}
    ${MCU_SPECIFIC_SOURCES}
    ${MCU_STARTUP_FILE}
)

# 9. 后处理
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_SIZE} ${PROJECT_NAME}.elf
)
