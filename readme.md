# GD32F303 CMake 工程

基于 GD32F303 的嵌入式开发框架，采用 CMake 构建系统。

## 目录结构

```
gd32F303_Cmake/
├── platform/                    # 平台层（芯片 SDK / HAL / 驱动 / 工具组件）
│   ├── chip/gd32f30x/          # 芯片支持包（sdk/startup/linker）
│   ├── hal/                    # 硬件抽象层（inc/src）
│   ├── drivers/                # 第三方驱动（当前含 sfud）
│   └── utils/                  # 通用工具（RTT 日志等）
│
├── boards/demo_board/          # 板级支持包
├── app/demo_flash/             # 应用代码（main 在 src/main.c）
├── tools/                      # 开发工具与脚本
├── CMakeLists.txt              # 唯一 CMake 入口（统一聚合所有源码）
└── arm-none-eabi.cmake         # 工具链配置
```

> 说明：当前仓库**不使用** `platform/CMakeLists.txt` 子目录编排，
> 平台层源码由顶层 `CMakeLists.txt` 统一收集与链接。

## 快速开始

### 环境要求

- CMake 3.16+
- ARM GCC 工具链 (arm-none-eabi-gcc)
- OpenOCD (用于烧录调试)

### 编译

```bash
mkdir build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../arm-none-eabi.cmake ..
make
```

### 烧录

```bash
openocd -f tools/gd32f303_cmsisdap.cfg -c "program build/Platform_Base_Project.elf verify reset exit"
```

## CI/CD

仓库已提供 GitHub Actions 自动化流程（`.github/workflows/ci.yml`），支持 `push` / `pull_request` / 手动触发：

- **CMake + GCC 构建**：在 Ubuntu 环境安装 `gcc-arm-none-eabi`，使用 CMake + Ninja 完成交叉编译，并上传产物。
- **代码审查**：使用 `cppcheck` 进行静态检查，执行报告脚本单元测试后，在 `review/code-review-report.html` 生成中文 HTML 审查报告并上传构件。

## 架构说明

采用分层架构设计：

- **platform 层**：芯片 SDK、HAL、驱动与通用工具。
- **boards 层**：板级引脚与外设映射。
- **app 层**：业务逻辑与应用入口。

当前构建入口为仓库根目录 `CMakeLists.txt`，统一聚合上述目录源码并生成固件。

## GPIO 配置方式（推荐）

当前 GPIO 采用 **X-Macro 单一数据源**，在
`boards/demo_board/config/gpio_table.def` 中维护：

- `platform/hal/inc/hal_gpio.h` 自动展开 GPIO ID 枚举
- `platform/hal/src/hal_gpio.c` 自动展开 GPIO 配置数组并在 BOARD 阶段自动初始化

这样无需 JSON/Python 生成步骤，配置与代码同源、可直接编译期检查。

## SPI 配置方式（X-Macro）

当前 SPI 也采用 **X-Macro 单一数据源**，在
`boards/demo_board/config/spi_table.def` 中维护：

- `platform/hal/inc/hal_spi.h` 自动展开 SPI ID 枚举
- `platform/hal/src/hal_spi.c` 自动展开 SPI 设备配置并完成 BOARD 阶段初始化
- `platform/hal/src/gd32f30x/port_spi.c` 提供 GD32F30x 端口实现（SPI1/SPI2）

示例（当前工程）：

- `EXT_FLASH -> SPI1 + PE2(CS)`

后续新增 SPI 外设时，只需在 `spi_table.def` 增加 `SPI_ITEM(...)`。

## SFUD 集成说明

工程已集成 SFUD，目录：

- `platform/drivers/sfud/inc`
- `platform/drivers/sfud/src`
- `platform/drivers/sfud/port`

当前适配关系：

- `platform/drivers/sfud/port/sfud_port.c` 通过 `hal_spi` 适配 SFUD 的 `spi.wr`
- 片选由 `hal_spi_id_cs_select/release` 控制
- 日志通过 `platform/utils/rtt_log` 输出

配置文件：

- `platform/drivers/sfud/inc/sfud_cfg.h`
  - 已配置设备索引 `SFUD_EXT_FLASH_DEVICE_INDEX`
  - 设备名为 `EXT_FLASH`
  - 启用 `SFUD_USING_SFDP` 与 `SFUD_USING_FLASH_INFO_TABLE`

## Demo 测试（SFUD）

`app/demo_flash/src/main.c` 已切换为 SFUD 测试流程：

1. 调用 `sfud_init()`
2. 通过 `sfud_get_device(SFUD_EXT_FLASH_DEVICE_INDEX)` 获取外部 Flash
3. 打印 JEDEC 与容量信息
4. 读取地址 `0x00000000` 起始的 16 字节并打印

典型成功日志包含：

- `Start initialize Serial Flash Universal Driver(SFUD)`
- `manufacturer ID/type ID/capacity ID`
- `Check SFDP header is OK`
- `Capacity: ...`

说明 SPI 与 SFUD 端口链路已打通。

## 日志系统（RTT）与日志等级

当前工程日志基于 `platform/utils/rtt_log_compat.h` 的 `LOG_E/LOG_W/LOG_I/LOG_D` 宏。

日志等级由 `LOG_LEVEL` 控制，含义如下：

- `0 = 全关`（`LOG_LEVEL_NONE`）
- `1 = 只看错误`（`LOG_LEVEL_ERROR`）
- `2 = 错误 + 警告`（`LOG_LEVEL_WARN`）
- `3 = 错误 + 警告 + 信息`（`LOG_LEVEL_INFO`）
- `4 = 全开（含调试）`（`LOG_LEVEL_DEBUG`）

默认配置为：

```c
#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_DEBUG
#endif
```

即默认输出全部等级日志。若需要降低日志量，可在项目编译选项或公共头文件中将 `LOG_LEVEL` 改为 `LOG_LEVEL_INFO` / `LOG_LEVEL_WARN` 等。

## 贡献指南

- 所有文本文件（如 `*.c`、`*.h`、`CMakeLists.txt`、`*.md`）统一使用 **UTF-8** 编码保存，避免出现乱码与编码漂移。
