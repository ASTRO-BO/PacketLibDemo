cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(PacketLibDemo)
set(PacketLibDemo_MAJOR_VERSION 1)
set(PacketLibDemo_MINOR_VERSION 0)
set(PacketLibDemo_PATCH_VERSION 0)
set(PacketLibDemo_VERSION
${PacketLibDemo_MAJOR_VERSION}.${PacketLibDemo_MINOR_VERSION}.${PacketLibDemo_PATCH_VERSION})
set(PROJECT_VERSION ${PacketLibDemo_VERSION})

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake-modules)

add_subdirectory(code)
