cmake_minimum_required(VERSION 3.0)
project(algoticks VERSION 1.0 LANGUAGES C)
set(CMAKE_C_STANDARD 11)

#create required dirs
file(MAKE_DIRECTORY "bin")
file(MAKE_DIRECTORY "bin/algorithms/")

include_directories(include /usr/include)
include_directories(${PROJECT_SOURCE_DIR})

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
file(COPY ${PROJECT_SOURCE_DIR}/assets/config.json
     DESTINATION bin/)
file(COPY ${PROJECT_SOURCE_DIR}/assets/settings.json
     DESTINATION bin/)
file(COPY ${PROJECT_SOURCE_DIR}/assets/benchmark.json
     DESTINATION bin/)