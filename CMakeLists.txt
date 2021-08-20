cmake_minimum_required(VERSION 3.0)
project(algoticks VERSION 1.0 LANGUAGES C)
set(CMAKE_C_STANDARD 11)

#uncomment below line to enable Optimizations
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DCHOMP -DQUOTED_CHECK -DCHECK_ROW_INTEGRITY")

if (NOT CMAKE_BUILD_TYPE)
     message("No Build type specified. Default is Debug.")
     set(CMAKE_BUILD_TYPE Debug)
endif()

if (CMAKE_BUILD_TYPE MATCHES Release)
     message("Release Type = Release")
     set(CMAKE_CURRENT_BINARY_DIR "release")
     set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s -O3")
endif()

#if debug in arg, change folder name to "debug"
if(CMAKE_BUILD_TYPE MATCHES Debug)
     message("Release Type = Debug")
     set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
     set(CMAKE_CURRENT_BINARY_DIR "debug")
ENDIF(CMAKE_BUILD_TYPE MATCHES Debug)


include_directories(include /usr/include)

#GLOB all source files!
file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.c)


#############ALGORITHMS#########################

#GLOB all source files!
file(GLOB ALGORITHMS ${PROJECT_SOURCE_DIR}/src/algorithms/*.c)

foreach(algo ${ALGORITHMS})
     get_filename_component(algo_name ${algo} NAME_WLE)
     message("Algorithm:" ${algo_name})
     add_library(${algo_name} MODULE src/algorithms/${algo_name}.c)
     set_target_properties(${algo_name}
                           PROPERTIES
                           PREFIX ""
                           LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/algorithms/
     )
endforeach()

###############//END//#########################

#############CALLBACKS#########################

#GLOB all source files!
file(GLOB CALLBACKS ${PROJECT_SOURCE_DIR}/src/callbacks/*.c)

foreach(cb ${CALLBACKS})
     get_filename_component(cb_name ${cb} NAME_WLE)
     message("Callback:" ${cb_name})
     add_library(${cb_name} MODULE src/callbacks/${cb_name}.c)
     set_target_properties(${cb_name}
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