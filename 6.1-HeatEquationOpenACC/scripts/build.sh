#!/usr/bin/env bash
set -e

GPU=${1:-ON}
MULTICORE=${2:-ON}

mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DUSE_GPU=$GPU -DUSE_MULTICORE=$MULTICORE
make -j