cmake_minimum_required(VERSION 3.0)
project(algoticks VERSION 1.0 LANGUAGES C)
set(CMAKE_C_STANDARD 11)

#default
set(out_folder_name "release")

#if debug in arg, change folder name to "debug"
if(CMAKE_BUILD_TYPE MATCHES Debug)
     set(out_folder_name "debug")
ENDIF(CMAKE_BUILD_TYPE MATCHES Debug)

#create required dirs
file(MAKE_DIRECTORY ${out_folder_name})
file(MAKE_DIRECTORY ${out_folder_name}/algorithms/)

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
    LIBRARY_OUTPUT_DIRECTORY ${out_folder_name}/algorithms/
)
endforeach()

add_executable(algoticks ${SRC_FILES})

#link required libs
target_link_libraries(algoticks PUBLIC json-c)
target_link_libraries(algoticks PUBLIC dl)

#set output to bin/
set_target_properties(algoticks
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${out_folder_name}/archive/
    LIBRARY_OUTPUT_DIRECTORY ${out_folder_name}/lib/
    RUNTIME_OUTPUT_DIRECTORY ${out_folder_name}/
)

#copy configs to bin/
file(COPY ${PROJECT_SOURCE_DIR}/assets/config.json
     DESTINATION ${out_folder_name}/)
file(COPY ${PROJECT_SOURCE_DIR}/assets/settings.json
     DESTINATION ${out_folder_name}/)
file(COPY ${PROJECT_SOURCE_DIR}/assets/benchmark.json
     DESTINATION ${out_folder_name}/)