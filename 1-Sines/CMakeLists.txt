cmake_minimum_required(VERSION 3.10)
project(SineSum)

set(CMAKE_CXX_COMPILER "/usr/bin/g++-11")

option(USE_FLOAT "Use float precision instead of double" OFF)

if(USE_FLOAT)
    add_definitions(-DUSE_FLOAT)
    message(STATUS "Using float precision.")
else()
    message(STATUS "Using double precision.")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(sine_sum main.cpp)
target_link_libraries(sine_sum m)