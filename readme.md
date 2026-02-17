# MCU 代码解耦与平台化架构设计参考指南



**核心架构思想：** 隔离变化，定义契约。将容易发生变化的硬件底层，与相对稳定的业务逻辑彻底解耦。

## 一、 平台化物理目录树搭建 (Directory Structure)

这是一个标准化的四层架构目录，请在建立相应目录或完成相应模块的初步搭建后，勾选对应的复选框。

### 1. 平台侧核心资产 (`platform/` - 多项目共享)

- [ ] **L0: Chip Support 层 (芯片原生支持)**
  - [ ] `chip/<mcu_series>/startup/`: 存放特定 MCU 的启动文件 (`startup_xxx.s`)。
  - [ ] `chip/<mcu_series>/linker/`: 存放特定 MCU 的链接脚本 (`xxx.ld`)。
  - [ ] `chip/<mcu_series>/sdk/`: 存放厂商原厂库 (如 GD32 标准库、STM32 HAL 库)。
- [ ] **L1: MHAL 层 (MCU 硬件抽象层)**
  - [ ] `mhal/inc/`: 建立平台统一的硬件接口定义 (如 `i_uart.h`, `i_i2c.h`)。
  - [ ] `mhal/src/<mcu_series>/`: 实现特定芯片的适配代码 (调用 L0 层的 SDK)。
- [ ] **L2b: Drivers 层 (设备驱动层)**
  - [ ] `drivers/sensor/`: 建立纯逻辑驱动代码 (如温湿度、加速度计，仅依赖 `mhal/inc/`)。
  - [ ] `drivers/display/`: 建立屏幕驱动代码。
- [ ] **L3: SSML 层 (系统服务与中间件层)**
  - [ ] `middleware/osal/`: 建立操作系统抽象层 (封装 RTOS API 或提供裸机轮询机制)。
  - [ ] `middleware/event_bus/`: 建立事件总线机制 (用于业务模块间的彻底解耦)。
  - [ ] `middleware/third_party/`: 集成第三方库 (如 FatFS, LwIP 等)。
- [ ] **L5: Common 层 (公共基础组件)**
  - [ ] `common/log/`: 实现统一的日志打印模块。
  - [ ] `common/utils/`: 实现基础工具类 (如 RingBuffer, 队列, CRC校验等)。
- [ ] **平台构建配置**
  - [ ] `platform/CMakeLists.txt`: 编写平台总构建脚本。
  - [ ] `platform/Kconfig`: 建立平台级图形化配置入口 (可选，用于模块裁剪)。

### 2. 应用侧具体产品 (`product_xxx/` - 特定业务逻辑)

- [ ] **L2a: BSP 层 (板级支持包)**
  - [ ] `bsp/board_config/`: 定义当前产品的引脚映射、时钟初始化配置。
- [ ] **L4: App 层 (业务应用层)**
  - [ ] `app/state_machine/`: 建立主状态机逻辑。
  - [ ] `app/ui/`: 建立交互与界面逻辑。
  - [ ] `app/data_process/`: 建立核心数据处理算法。
  - [ ] **架构红线检查：** 确保 `app/` 目录下没有任何底层的硬件头文件 (如 `#include "stm32f1xx.h"`)。
- [ ] **产品构建配置**
  - [ ] `product_xxx/CMakeLists.txt`: 编写产品级构建脚本，按需链接 platform 中的模块。
  - [ ] `product_xxx/main.c`: 编写程序入口，完成各层级的初始化与依赖注入。

---

## 二、 核心解耦实现步骤 (Implementation Steps)

在进行具体代码编写时，请遵循以下四个核心步骤，确保代码真正实现解耦。

### 第一阶段：定义契约 (面向接口编程)
- [ ] **梳理硬件需求：** 确定系统需要哪些底层外设 (UART, I2C, SPI, ADC, Timer 等)。
- [ ] **定义虚函数表：** 在 `mhal/inc/` 中，使用结构体包含函数指针的方式，为每一个外设定义标准接口。
  > *示例：定义 `iI2C_Interface_t`，包含 `Init`, `WriteReg`, `ReadReg` 等指针。*

### 第二阶段：编写纯逻辑驱动 (依赖注入)
- [ ] **去除硬件依赖：** 编写外部设备驱动时，移除所有特定芯片厂商的库头文件。
- [ ] **接口注入：** 在驱动模块的初始化函数中，要求传入第一阶段定义的接口结构体指针。
  > *示例：`int SHTC3_Init(SHTC3_Device_t *dev, iI2C_Interface_t *i2c_interface)`。*
- [ ] **基于接口操作：** 驱动模块内部的所有硬件操作，全部通过注入的接口指针来调用。

### 第三阶段：板级绑定与组装 (BSP 与 Main)
- [ ] **实现底层适配：** 在 `mhal/vendor_a/` 中，利用原厂库 (如 HAL 库) 实现第一阶段定义的接口结构体。
- [ ] **顶层装配：** 在 `main.c` 或 BSP 初始化阶段，将具体的底层实现实例，传递给上层的逻辑驱动实例，完成绑定。

### 第四阶段：构建系统的强制约束
- [ ] **模块化编译：** 在 CMake 中使用 `add_library` 将各层定义为独立目标。
- [ ] **控制依赖范围：** 严格使用 `target_link_libraries` 声明依赖关系。例如，强制应用层 (`app`) 只能链接服务层 (`middleware`) 和接口定义 (`mhal_inc`)，从编译层面上掐断越级调用底层硬件代码的可能性。