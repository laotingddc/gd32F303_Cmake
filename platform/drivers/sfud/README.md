# SFUD source drop-in directory

把 SFUD 原始源码放到这个目录即可（你来拷贝）。

建议至少包含：

- `sfud.c`
- `sfud.h`
- `sfud_def.h`
- `sfud_cfg.h`（可按项目需要修改）

说明：

- 顶层 `CMakeLists.txt` 已加入 `platform/drivers/sfud` 头文件路径。
- `platform/drivers/**/*.c` 会被自动编译，所以你放入的 `*.c` 会自动参与构建。
- 后续我再帮你补 `sfud` 与 `mhal_spi` 的适配层（如 `sfud_port.c` / `sfud_adapter.c`）。
