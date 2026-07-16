cmake_minimum_required(VERSION 4.0)
project(VibrantLogs VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(timey GIT_REPOSITORY https://github.com/loganjellis/Timey.git GIT_TAG main)
FetchContent_MakeAvailable(timey)

add_library(vibrant_logs "${CMAKE_CURRENT_SOURCE_DIR}/src/vibrant_logs.c")

option(BUILD_SHARED_LIBS "Build using a shared library." ON)
target_compile_definitions(vibrant_logs PRIVATE $<$<BOOL:${BUILD_SHARED_LIBS}>:VL_EXPORTS>)

target_include_directories(vibrant_logs PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(vibrant_logs PUBLIC timey)
