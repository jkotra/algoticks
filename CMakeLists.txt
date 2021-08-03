cmake_minimum_required(VERSION 3.0)
project(algoticks VERSION 1.0 LANGUAGES C)
set(CMAKE_C_STANDARD 11)

set(CMAKE_BUILD_TYPE Debug)
if (CMAKE_BUILD_TYPE MATCHES Release)
    message("Release Build: Enabling Optimizations (-O3)")
    set(CMAKE_C_FLAGS "-O3")
endif()

#default
set(CMAKE_CURRENT_BINARY_DIR "release")

#if debug in arg, change folder name to "debug"
if(CMAKE_BUILD_TYPE MATCHES Debug)
     set(CMAKE_C_FLAGS "-Wall")
     set(CMAKE_CURRENT_BINARY_DIR "debug")
ENDIF(CMAKE_BUILD_TYPE MATCHES Debug)


include_directories(include /usr/include)

#GLOB all source files!
file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.c)

#############ALGORITHMS#########################

# add algorithm names to this list
list(APPEND algorithms
    Greens
    Reds
    ThreeLineStrike
    ThreeBlackCrows
    TwoBlackGapping
    EveningStar
    AbandonedBaby)

foreach(algo ${algorithms})
message("Building Algorithm:" ${algo})
add_library(${algo} MODULE src/algorithms/${algo}.c)
set_target_properties(${algo}
    PROPERTIES
    PREFIX ""
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/algorithms/
)
endforeach()

###############//END//#########################

#############CALLBACKS#########################

# add algorithm names to this list
list(APPEND callbacks
     log)

foreach(cb ${callbacks})
message("Building callback:" ${cb})
add_library(${cb} MODULE src/callbacks/${cb}.c)
set_target_properties(${cb}
    PROPERTIES
    PREFIX ""
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/callbacks/
)
endforeach()

###############//END//######################


add_executable(algoticks ${SRC_FILES})

#link required libs
target_link_libraries(algoticks PRIVATE json-c)
target_link_libraries(algoticks PRIVATE zmq)
target_link_libraries(algoticks PRIVATE uv)

if(NOT WIN32)
     target_link_libraries(algoticks PRIVATE dl)
endif()

#set output to bin/
set_target_properties(algoticks
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/archive/
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib/
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/
)

if(WIN32)
set(config_dir ${PROJECT_SOURCE_DIR}/assets/configs/windows)
else()
set(config_dir ${PROJECT_SOURCE_DIR}/assets/configs/linux)
endif()

#copy configs to bin/
file(COPY ${config_dir}/config.json
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
file(COPY ${config_dir}/settings.json
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
file(COPY ${config_dir}/benchmark.json
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
     file(COPY ${PROJECT_SOURCE_DIR}/assets/example.csv
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)