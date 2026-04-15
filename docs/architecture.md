# 工程架构说明（简版）

本文档面向新同事，目标是 **10 分钟内看懂工程结构与扩展方式**。

## 1. 分层模型

工程采用四层结构：

- **projects/**：产品层（选择 board/app、产品级编译配置）
- **app/**：应用层（业务流程与应用入口）
- **boards/**：板级层（引脚/外设映射、板级初始化）
- **platform/**：平台层（芯片 SDK、HAL、驱动、工具组件）

简图：

```
app  --> hal --> chip sdk
  \      |
   \--> drivers (如 SFUD)

boards 提供硬件资源映射（gpio_table/spi_table）
projects 决定“用哪个 board + 哪个 app”
```

## 2. 启动与初始化流程

1. `Reset_Handler`（startup）
2. `SystemInit`
3. `__libc_init_array`
4. `auto_init` 遍历 `.init_fn.*`
5. `main`

当前 BOARD 阶段典型顺序：

- `board_hw_init`（默认优先级 50）
- `hal_gpio_init_from_cfg`（优先级 60）
- `hal_spi_init_from_cfg`（优先级 70）

## 3. 配置单一数据源（X-Macro）

- GPIO：`boards/<board>/config/gpio_table.def`
- SPI：`boards/<board>/config/spi_table.def`

`hal_gpio.h/.c`、`hal_spi.h/.c` 会展开这些配置，避免手写重复枚举和配置表。

## 4. 构建入口与选择关系

顶层 `CMakeLists.txt` 支持：

- `PRODUCT`（默认 `demo_product`）
- `BOARD`（可覆盖）
- `APP`（可覆盖）

默认值来自 `projects/<product>/project.cmake`。

示例：

```bash
cmake -DPRODUCT=demo_product -DBOARD=demo_board -DAPP=demo_flash ..
```

## 5. 扩展指南（最小步骤）

### 新增一个 app

1. 新建 `app/<new_app>/inc` 与 `src`
2. 在 `src/main.c` 中仅保留应用入口调度
3. 将业务逻辑放到独立 `app_<name>.c`
4. 通过 `-DAPP=<new_app>` 选择

### 新增一个 board

1. 新建 `boards/<new_board>/board.c/.h`
2. 提供 `config/gpio_table.def` 与 `config/spi_table.def`
3. 通过 `-DBOARD=<new_board>` 选择
