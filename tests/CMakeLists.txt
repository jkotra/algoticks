cmake_minimum_required(VERSION 3.0)
project(algoticks_tests VERSION 1.0 LANGUAGES C)
set(CMAKE_C_STANDARD 11)

add_executable(algoticks_tests algoticks_tests.c ../src/misc.c ../src/dashboard.c ../src/timeutils.c ../src/csvutils.c ../src/debug.c)

target_link_libraries(algoticks_tests PUBLIC check)
target_link_libraries(algoticks_tests PUBLIC json-c)

if (NOT WIN32)
target_link_libraries(algoticks_tests PUBLIC dl)
endif()