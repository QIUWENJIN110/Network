#!/bin/bash

echo "=== Configuring CMake ==="

cmake \
  -S . \
  -B build \
  -DCMAKE_BUILD_TYPE=Debug

echo "=== Configure Complete ==="
