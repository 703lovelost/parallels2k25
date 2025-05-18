cmake_minimum_required(VERSION 3.17)
project(heat2d)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS program_options)

option(USE_GPU "Build with GPU support" ON)
option(USE_MULTICORE "Build multicore-version for CPU" OFF)

set(CMAKE_CXX_COMPILER nvc++)

set(CUDA_HOME "/opt/nvidia/hpc_sdk/Linux_x86_64/24.5/cuda/12.4")

if(USE_GPU)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -acc -gpu=cc89 -stdpar=gpu -Minfo=accel -O3")
    message(STATUS "Building with Tesla support")
else()
  if(USE_MULTICORE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -acc=multicore -Minfo=all -fast")
    message(STATUS "Building CPU-multicore version")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -acc=host -Minfo=all -fast")
    message(STATUS "Building CPU-onecore version")
  endif()
endif()

add_executable(heat2d src/main.cpp src/heat_solver.cpp src/boundary.cpp)

target_include_directories(heat2d PRIVATE include)
target_link_libraries(heat2d PRIVATE Boost::program_options)

target_include_directories(heat2d PRIVATE
  ${CUDA_HOME}/include
)
target_link_directories(heat2d PRIVATE
  ${CUDA_HOME}/lib64
)
target_link_libraries(heat2d PRIVATE cudart)