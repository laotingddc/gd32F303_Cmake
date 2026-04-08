# Reusable init + RTT package

This folder is intended to be copied into new CMake-based MCU projects.

## Contents

- `auto_init.h`: section-based auto registration macros.
- `rtt_log.h`: minimal RTT logging API.
- `rtt_log_compat.h`: optional `LOG_E/LOG_I/...` shim for existing code.
- `SEGGER_RTT*.c/.h`: vendored RTT implementation.

## Integration notes

- Keep the startup file calling `auto_init` after `__libc_init_array`.
- Keep the linker script section for `.init_fn.*` and the symbols `__init_fn_start` / `__init_fn_end`.
- Add this folder to the include path.
- Add the `*.c` files in this folder to the project source list.
- `RTT_USE_ASM` is disabled by default to keep the package toolchain-light.
