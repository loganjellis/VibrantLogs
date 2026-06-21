cmake_minimum_required(VERSION 4.0)
project(VibrantLogs VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_COMPILER "C:/ProgramData/mingw64/mingw64/bin/gcc.exe")

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# option for building shared lib (defaults to ON, use -DBUILD_SHARED_LIBS=OFF to build a static library)
option(BUILD_SHARED_LIBS "Build using a shared library." ON)

# source code
set(LIB_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/vibrant_logs.c")

# building shared lib:
add_library(vibrant_logs ${LIB_SRC})

# VL_EXPORTS must be defined when building VibrantLogs as a shared lib
target_compile_definitions(vibrant_logs PRIVATE $<$<BOOL:${BUILD_SHARED_LIBS}>:VL_EXPORTS>)

# include dirs
target_include_directories(vibrant_logs PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# add Timey to VibrantLogs
target_link_libraries(vibrant_logs PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/lib/libtimey.dll.a")

# executable for testing!
add_executable(VibrantLogs_test "${CMAKE_CURRENT_SOURCE_DIR}/example.c")
target_link_libraries(VibrantLogs_test PRIVATE vibrant_logs)

# installing VibrantLogs
install(TARGETS vibrant_logs
	EXPORT VibrantLogsTargets
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT VibrantLogsTargets
	FILE VibrantLogsTargets.cmake
	NAMESPACE VibrantLogs::
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/VibrantLogs)

configure_package_config_file(
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/VibrantLogsConfig.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/VibrantLogsConfig.cmake
	INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/VibrantLogs)

write_basic_package_version_file(
	${CMAKE_CURRENT_BINARY_DIR}/VibrantLogsConfigVersion.cmake
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion)

install(FILES
	${CMAKE_CURRENT_BINARY_DIR}/VibrantLogsConfig.cmake
	${CMAKE_CURRENT_BINARY_DIR}/VibrantLogsConfigVersion.cmake
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/VibrantLogs)
