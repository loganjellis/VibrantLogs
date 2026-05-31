<div align="center">
    <img src="./logo.png" alt="VibrantLogs logo">
</div>

# VibrantLogs

**VibrantLogs** is a simple logging library in C with colored terminal output and customizable color schemes.

## Features

- Colored terminal output
- Fully customizable RGB colors
- Multiple log levels
- Delayed log messages

> [!NOTE]
> VibrantLogs is not thread-safe as of right now.

## Clone the repository

```
git clone https://github.com/loganjellis/VibrantLogs.git
cd VibrantLogs
```

## Building (and optionally installing)

```
cmake -S . -B build
cmake --build build
(only include this line if you want to install VibrantLogs) cmake --install build --prefix ./install
```

> [!NOTE]
> Note that ./install is a placeholder install location for the library. Omitting the install location results in the library being installed in the operating system's default path.

### Using VibrantLogs (installed package)

```
list(APPEND CMAKE_PREFIX_PATH "/path/to/vibrant_logs/install")
find_package(VibrantLogs REQUIRED)
target_link_libraries(app PRIVATE VibrantLogs::vibrant_logs)
```

### Using VibrantLogs (subdirectory)

```
add_subdirectory(VibrantLogs)
target_link_libraries(app PRIVATE VibrantLogs::vibrant_logs)
```

## Example

For an example of basic usage of the library, refer to `example.c`:

```c
#include "vibrant_logs.h"

int main(void)
{
	// init VibrantLogs!
	vl_init();

	// you can edit the color scheme of VibrantLogs:
	vl_color_scheme *vl_colors = vl_curr_colors();
	vl_colors -> time_color = (vl_color) { 150, 150, 255 };
	vl_colors -> date_color = (vl_color) { 175, 175, 255 };
	vl_colors -> debug_prefix_color = (vl_color) { 200, 100, 100 };
	vl_colors -> debug_color = (vl_color) { 225, 150, 150 };

    // update config to print both time and date
	vl_config *vl_cfg = vl_curr_config();
	vl_cfg -> print_time = true;
	vl_cfg -> print_date = true;

	// testing each log type:
	vl_log(VL_INFO, "Info test.");
	vl_log(VL_SUCCESS, "Success!");
	vl_log(VL_DEBUG, "Debugging...");
	vl_log(VL_WARNING, "Warning.");
	vl_log(VL_ERROR, "ERROR!");

	return 0;
}```

## Documentation and Information

Please refer to the VibrantLogs documentation <a href="docs/html" target="_blank">here</a>.
