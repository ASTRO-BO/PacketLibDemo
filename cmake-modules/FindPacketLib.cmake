# - Try to find WCTOOLS
# Once done, this will define
#
#  PacketLib_FOUND - system has found libwcs
#  PacketLib_INCLUDE_DIRS - the PacketLib include directories
#  PacketLib_LIBRARIES - link these to use PacketLib
#
# See documentation on how to write CMake scripts at
# http://www.cmake.org/Wiki/CMake:How_To_Find_Libraries

include(LibFindMacros)

libfind_pkg_check_modules(PacketLib_PKGCONF libpacket)

find_path(PacketLib_INCLUDE_DIR
  NAMES packet/PacketLibDefinition.h
  PATHS ${PacketLib_PKGCONF_INCLUDE_DIRS}
)

find_library(PacketLib_LIBRARIES
  NAMES packet
  PATHS ${PacketLib_PKGCONF_LIBRARY_DIRS}
)

set(PacketLib_PROCESS_INCLUDES PacketLib_INCLUDE_DIR)
set(PacketLib_PROCESS_LIBS PacketLib_LIBRARIES)
libfind_process(PacketLib)
