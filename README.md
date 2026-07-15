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

## Documentation and Information

Please refer to the VibrantLogs documentation <a href="docs/html" target="_blank">here</a>.
