cmake_minimum_required(VERSION 4.0)
project(VibrantLogs VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_COMPILER "C:/ProgramData/mingw64/mingw64/bin/gcc.exe")

option(BUILD_SHARED_LIBS "Build using a shared library." ON)

add_library(vibrant_logs "${CMAKE_CURRENT_SOURCE_DIR}/src/vibrant_logs.c")

target_compile_definitions(vibrant_logs PRIVATE $<$<BOOL:${BUILD_SHARED_LIBS}>:VL_EXPORTS>)

target_include_directories(vibrant_logs PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(vibrant_logs PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/lib/libtimey.dll.a")

add_executable(vibrant_logs_test "${CMAKE_CURRENT_SOURCE_DIR}/example.c")
target_link_libraries(vibrant_logs_test PRIVATE vibrant_logs)
