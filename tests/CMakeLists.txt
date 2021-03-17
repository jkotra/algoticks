cmake_minimum_required(VERSION 3.0)
project(algoticks_tests VERSION 1.0 LANGUAGES C)
set(CMAKE_C_STANDARD 11)

if (WIN32)
include_directories(C:/vcpkg/installed/x64-windows/include)
link_directories(C:/vcpkg/installed/x64-windows/lib)
endif()

add_executable(algoticks_tests algoticks_tests.c ../src/misc.c ../src/parser.c ../src/dashboard.c ../src/timeutils.c ../src/csvutils.c ../src/debug.c)

target_link_libraries(algoticks_tests PRIVATE check)
target_link_libraries(algoticks_tests PRIVATE json-c)
target_link_libraries(algoticks_tests PRIVATE zmq)

if(NOT WIN32)
target_link_libraries(algoticks_tests PRIVATE dl)
endif()