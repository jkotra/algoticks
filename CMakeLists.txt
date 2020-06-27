cmake_minimum_required(VERSION 3.0)
project(algoticks VERSION 0.1)

#create required dirs
file(MAKE_DIRECTORY "bin")
file(MAKE_DIRECTORY "bin/algorithms/")

include_directories(include /usr/include)
include_directories(${PROJECT_SOURCE_DIR})

find_package(json-c REQUIRED)

#GLOB all source files!
file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.c)

# add algorithm names to this list
list(APPEND algorithms
    3Greens
    3Reds)

foreach(algo ${algorithms})
message("Building Algorithm:" ${algo})
add_library(${algo} MODULE src/algorithms/${algo}.c)
set_target_properties(${algo}
    PROPERTIES
    PREFIX ""
    LIBRARY_OUTPUT_DIRECTORY "bin/algorithms/"
)
endforeach()

add_executable(algoticks ${SRC_FILES})

#link required libs
target_link_libraries(algoticks PUBLIC json-c)
target_link_libraries(algoticks PUBLIC dl)

#set output to bin/
set_target_properties(algoticks
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "bin/archive/"
    LIBRARY_OUTPUT_DIRECTORY "bin/lib/"
    RUNTIME_OUTPUT_DIRECTORY "bin/"
)

#copy configs to bin/
file(COPY ${PROJECT_SOURCE_DIR}/example_configs/config.json
     DESTINATION bin/)
file(COPY ${PROJECT_SOURCE_DIR}/example_configs/settings.json
     DESTINATION bin/)
file(COPY ${PROJECT_SOURCE_DIR}/example_configs/benchmark.json
     DESTINATION bin/)