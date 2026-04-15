cmake_minimum_required(VERSION 3.10)

# 1. 工具链设置
if(NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/arm-none-eabi.cmake")
endif()
include(${CMAKE_TOOLCHAIN_FILE})

# 2. 项目定义
project(Platform_Base_Project C ASM)

# 产品/板级/应用选择
set(PRODUCT demo_product CACHE STRING "Product name under projects/")
set(BOARD "" CACHE STRING "Board name under boards/ (optional override)")
set(APP "" CACHE STRING "App name under app/ (optional override)")

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/projects/${PRODUCT}")
    message(FATAL_ERROR "Product not found: projects/${PRODUCT}")
endif()

set(PRODUCT_CONFIG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/projects/${PRODUCT}/project.cmake")
if(EXISTS "${PRODUCT_CONFIG_FILE}")
    include("${PRODUCT_CONFIG_FILE}")
else()
    message(WARNING "Product config not found: ${PRODUCT_CONFIG_FILE}, fallback to defaults")
endif()

if(DEFINED PRODUCT_COMPILE_DEFINITIONS)
    add_definitions(${PRODUCT_COMPILE_DEFINITIONS})
endif()

set(_default_board demo_board)
set(_default_app demo_flash)

if(DEFINED PRODUCT_BOARD)
    set(_default_board "${PRODUCT_BOARD}")
endif()

if(DEFINED PRODUCT_APP)
    set(_default_app "${PRODUCT_APP}")
endif()

if(NOT BOARD)
    set(BOARD "${_default_board}" CACHE STRING "Board name under boards/" FORCE)
endif()

if(NOT APP)
    set(APP "${_default_app}" CACHE STRING "App name under app/" FORCE)
endif()

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/boards/${BOARD}")
    message(FATAL_ERROR "Board not found: boards/${BOARD}")
endif()

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/app/${APP}")
    message(FATAL_ERROR "App not found: app/${APP}")
endif()

message(STATUS "Build selection: PRODUCT=${PRODUCT}, BOARD=${BOARD}, APP=${APP}")

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
file(GLOB_RECURSE DRIVER_SOURCES "platform/drivers/*.c")

# HAL 通用层源码
set(HAL_COMMON_SOURCES
    platform/hal/src/hal_gpio.c
    platform/hal/src/hal_spi.c
)

file(GLOB APP_SOURCES "app/${APP}/src/*.c")
if(NOT APP_SOURCES)
    message(FATAL_ERROR "No app sources found under app/${APP}/src")
endif()

file(GLOB BOARD_SOURCES "boards/${BOARD}/*.c")
if(NOT BOARD_SOURCES)
    message(FATAL_ERROR "No board sources found under boards/${BOARD}")
endif()

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
