cmake_minimum_required(VERSION 3.18)
project(heat2d LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CUDA_STANDARD 17)
set(CMAKE_CUDA_ARCHITECTURES 70)

find_package(Boost REQUIRED COMPONENTS program_options)
find_package(CUDA REQUIRED)

set(CMAKE_CXX_COMPILER nvc++)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

include_directories(${Boost_INCLUDE_DIRS})

add_executable(heat2d
    src/heat2d_cuda.cu
    src/boundary.cpp
)

target_include_directories(heat2d PRIVATE include)

target_link_libraries(heat2d
    Boost::program_options
    cudart
)