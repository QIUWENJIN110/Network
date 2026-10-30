#!/bin/bash

echo "=== Building ==="

cmake --build build

if [ $? -ne 0 ]; then
    echo "Build Failed!"
    exit 1
fi

echo "=== Starting Server... ==="

./build/server