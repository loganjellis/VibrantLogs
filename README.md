# VibrantLogs

**VibrantLogs** is a logging library written in C. This repository contains the **header file** and precompiled **static and dynamic libraries**. The source code is closed.

The name VibrantLogs comes from the fact that the library prints in very bright colors. But the library also allows the user to set
custom colors using viblogs_color_scheme structs. The user can edit the prefix color as well as the main color of each message type.

For simplicity, the library is ***not*** cross-platform and is intended for use on Windows.

## Documentation and Information

Please refer to the VibrantLogs documentation <a href="docs/html/index.html" target="_blank">here</a>.

For an example of basic usage of the library, refer to `example.c`.

## Contents

- `include/` - Public headers (`vibrant_logs.h`)
- `lib/static/` - Static library (`vibrant_logs.lib`)
- `lib/dynamic/` - Dynamic library (`vibrant_logs.dll` + `vibrant_logs.lib`)
