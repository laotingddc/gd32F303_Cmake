cmake_minimum_required(VERSION 3.10)

# 1. 工具链设置
if(NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/arm-none-eabi.cmake")
endif()
include(${CMAKE_TOOLCHAIN_FILE})

# 2. 项目定义
project(Platform_Base_Project C ASM)

# 3. MCU 平台选择
if(NOT MCU_SERIES)
    set(MCU_SERIES "gd32f30x" CACHE STRING "Target MCU Series")
endif()

# 4. 加载芯片特定配置
include(cmake/mcu_${MCU_SERIES}.cmake)

# 5. 全局编译选项
set(CMAKE_C_FLAGS "${CPU_FLAGS} -Wall -fdata-sections -ffunction-sections" CACHE INTERNAL "")
set(CMAKE_ASM_FLAGS "${CPU_FLAGS} -x assembler-with-cpp" CACHE INTERNAL "")
set(CMAKE_EXE_LINKER_FLAGS "${CPU_FLAGS} -T${MCU_LINKER_SCRIPT} -Wl,--gc-sections --specs=nosys.specs --specs=nano.specs -Wl,-Map=${PROJECT_NAME}.map" CACHE INTERNAL "")

# 6. 包含目录
include_directories(
    platform/mhal/inc
    platform/common/log      # 包含 rtt_log.h 和 SEGGER_RTT.h
    platform/common/utils
    projects/demo_product/bsp/board_config
    ${MCU_SPECIFIC_INCLUDES}
)

# 7. 汇总源码
file(GLOB_RECURSE COMMON_SOURCES "platform/common/**/*.c")
file(GLOB_RECURSE DRIVER_SOURCES "platform/drivers/**/*.c")

# 产品特定源码
set(PRODUCT_SOURCES 
    projects/demo_product/main.c
    projects/demo_product/bsp/board_config/gd32f307c_eval.c
    projects/demo_product/bsp/board_config/board_init.c
)

# 8. 生成可执行文件
add_executable(${PROJECT_NAME}.elf 
    ${PRODUCT_SOURCES}
    ${COMMON_SOURCES}
    ${DRIVER_SOURCES}
    ${MCU_SPECIFIC_SOURCES}
    ${MCU_STARTUP_FILE}
)

# 9. 后处理
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_SIZE} ${PROJECT_NAME}.elf
)
