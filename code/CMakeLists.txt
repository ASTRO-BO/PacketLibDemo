# check dependencies
find_package(PacketLib REQUIRED)
include_directories(${PacketLib_INCLUDE_DIR})
set(LIBS ${LIBS} ${PacketLib_LIBRARIES})

# add -lrt on linux systems for clock_gettime
if (${UNIX})
    set(LIBS ${LIBS} rt)
endif (${UNIX})

# make
set(EXE0 packetlibdemo)
add_executable(${EXE0} mac_clock_gettime.cpp main.cpp packetlibop.cpp)
target_link_libraries(${EXE0} ${LIBS})

set(EXE1 exe1)
add_executable(${EXE1} exe1.cpp)
target_link_libraries(${EXE1} ${LIBS})

set(EXE2 exe2)
add_executable(${EXE2} exe2.cpp)
target_link_libraries(${EXE2} ${LIBS})

set(EXE3 exe3)
add_executable(${EXE3} exe3.cpp)
target_link_libraries(${EXE3} ${LIBS})

# make install
install(TARGETS ${EXE0} ${EXE1} ${EXE2} ${EXE3} DESTINATION bin)

# make uninstall
configure_file(
   "${CMAKE_CURRENT_SOURCE_DIR}/../cmake_uninstall.cmake.in"
   "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)
add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

# make docs
find_package(Doxygen)
if (NOT DOXYGEN_FOUND)
    message("Doxygen not found. Cannot generate make docs target.")
else ()
    configure_file(../Doxyfile.in ${PROJECT_BINARY_DIR}/Doxyfile @ONLY IMMEDIATE)
    add_custom_target(docs COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile)
endif (NOT DOXYGEN_FOUND)
