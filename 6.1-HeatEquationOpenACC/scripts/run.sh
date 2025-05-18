#!/usr/bin/env bash
set -e

SIZE=${1:-128}
TOL=${2:-1e-6}
MAXITER=${3:-1000000}
TARGET=${4:-host}

BUILD_DIR=build
if [[ ! -x "$BUILD_DIR/heat2d" ]]; then
  echo "Сначала выполните build.sh" >&2
  exit 1
fi

if [[ "$TARGET" == "gpu" ]]; then
  echo "Running on GPU"
  export ACC_DEVICE_TYPE=gpu
else
  echo "Running on HOST"
  export ACC_DEVICE_TYPE=host
fi

"$BUILD_DIR/heat2d" \
  --size "$SIZE" \
  --tol "$TOL" \
  --max-iter "$MAXITER"
