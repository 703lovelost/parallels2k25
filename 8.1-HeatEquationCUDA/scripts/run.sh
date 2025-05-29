#!/usr/bin/env bash
set -e

SIZE=${1:-128}
TOL=${2:-1e-6}
MAXITER=${3:-1000000}

BUILD_DIR=build
if [[ ! -x "$BUILD_DIR/heat2d" ]]; then
  echo "Сначала выполните build.sh" >&2
  exit 1
fi

"$BUILD_DIR/heat2d" \
  --size "$SIZE" \
  --tol "$TOL" \
  --max-iter "$MAXITER"
