# GD32F303 CMake 工程

基于 GD32F303 的嵌入式开发框架，采用 CMake 构建系统。

## 目录结构

```
gd32F303_Cmake/
├── platform/                    # 平台层 (硬件相关)
│   ├── chip/gd32f30x/          # GD32F30x 芯片支持
│   │   ├── sdk/                # 官方标准外设库
│   │   ├── startup/            # 启动文件
│   │   └── linker/             # 链接脚本
│   ├── mhal/                   # MCU 硬件抽象层
│   │   ├── inc/                # 统一接口定义
│   │   └── src/                # 接口实现
│   └── common/                 # 公共组件
│       ├── log/                # RTT 日志
│       └── utils/              # 工具函数
│
├── projects/                   # 项目层 (应用相关)
│   └── demo_product/           # 示例项目
│       ├── app/                # 应用逻辑
│       ├── bsp/                # 板级配置
│       ├── main.c              # 程序入口
│       └── CMakeLists.txt      # 项目构建配置
│
├── tools/                      # 开发工具
│   ├── GD32F30x_HD.svd         # 调试用 SVD 文件
│   ├── gd32f303_cmsisdap.cfg   # OpenOCD 配置
│   └── ...
│
├── CMakeLists.txt              # 顶层构建配置
├── arm-none-eabi.cmake         # 工具链配置
└── .vscode/                    # VSCode 配置
```

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
openocd -f tools/gd32f303_cmsisdap.cfg -c "program build/demo_product.elf verify reset exit"
```

## 架构说明

采用分层架构设计：

- **Platform 层**: 提供硬件抽象，包含芯片 SDK、HAL 接口定义及实现
- **Projects 层**: 具体产品应用，包含板级配置和业务逻辑

应用层通过 MHAL 接口访问硬件，实现业务代码与底层驱动的解耦。

## GPIO 配置方式（推荐）

当前 GPIO 采用 **X-Macro 单一数据源**，在
`projects/demo_product/bsp/board_config/mhal_gpio_table.def` 中维护：

- `platform/mhal/inc/mhal_gpio.h` 自动展开 GPIO ID 枚举
- `platform/mhal/src/mhal_gpio.c` 自动展开 GPIO 配置数组并在 BOARD 阶段自动初始化

这样无需 JSON/Python 生成步骤，配置与代码同源、可直接编译期检查。
