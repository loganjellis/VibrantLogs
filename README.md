# VibrantLogs

**VibrantLogs** is a logging library written in C. This repository contains all the necessary resources to use the VibrantLogs library.

The name VibrantLogs comes from the fact that the library prints in very bright colors. But the library also allows the user to set
custom colors using vl_color_scheme structs. The user can edit the prefix color as well as the main color of each message type.

## Installation

```
cmake -S . -B build
cmake --build build
cmake --install build --prefix ./install

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/install")
find_package(VibrantLogs REQUIRED)
target_link_libraries(app PRIVATE VibrantLogs::vibrant_logs)
```

Note that ./install is a placeholder install location for the library. Omitting the install
location results in the library being installed in the operating system's default path.

## Use as a subdirectory

```
add_subdirectory(VibrantLogs)
target_link_libraries(app PRIVATE VibrantLogs::vibrant_logs)
```

## Documentation and Information

Please refer to the VibrantLogs documentation <a href="docs/html" target="_blank">here</a>.

For an example of basic usage of the library, refer to `example.c`:

```c
#include "vibrant_logs.h"

int main(void)
{
	// you can edit the color scheme of VibrantLogs:
	vl_color_scheme mycolors = VL_DEFAULT_COLORS;
	mycolors.time_color = (vl_color) { 150, 150, 255 };
	mycolors.debug_prefix_color = (vl_color) { 200, 100, 100 };
	mycolors.debug_color = (vl_color) { 225, 150, 150 };

	// remember to set the color scheme using vl_set_colors(color_scheme):
	vl_set_colors(mycolors);


	// test out different log levels:

	// vl_set_log_level(VL_INFO); // default log level, shows all messages
	// vl_set_log_level(VL_WARNING); // shows warnings and errors
	// vl_set_log_level(VL_ERROR); // shows only errors

	// note that VL_DEBUG and VL_SUCCESS have no effect when used in vl_set_log_level(...).


	// testing each log type:
	vl_log(VL_INFO, "Info test.");
	vl_log(VL_SUCCESS, "Success!");
	vl_log(VL_DEBUG, "Debugging...");
	vl_log(VL_WARNING, "Warning.");
	vl_log(VL_ERROR, "ERROR!");
}
```
