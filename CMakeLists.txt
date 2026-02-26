cmake_minimum_required(VERSION 3.21)

project(FB LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(FB_CORE)

include(Engine/CMakeLists.txt)
include(Feed/CMakeLists.txt)
list(APPEND FB_CORE
${CMAKE_CURRENT_LIST_DIR}/Utils.cpp
)

set(FB_TARGET fb)

set(FB_SOURCES
    ${FB_CORE}
    main.cpp)

add_executable(${FB_TARGET} ${FB_SOURCES})
target_include_directories(${FB_TARGET} PUBLIC ${PROJECT_SOURCE_DIR})

set_target_properties(${FB_TARGET} PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)

if (TEST)
    add_subdirectory("Test")
endif()

if (BENCH)
    add_subdirectory("Benchmark")
endif()