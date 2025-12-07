#!/bin/bash
#
# Usage: ./scripts/run.sh [trace]
# Run from project root

TRACE_FLAG=""
if [[ "$1" == "trace" ]]; then
    TRACE_FLAG="-DTRACE"
fi

TARGET="ll_isort"
SRC_DIR="$(dirname "$0")/../src"
INC_DIR="$(dirname "$0")/../include"

# Try clang++ first (default on macOS)
if command -v clang++ &> /dev/null; then
    echo "Building with clang++ $TRACE_FLAG"
    clang++ -std=c++20 -O2 -Wall -Wextra -pedantic -I"$INC_DIR" $TRACE_FLAG "$SRC_DIR/main.cpp" -o "$TARGET"
    if [[ $? -ne 0 ]]; then
        echo "Build failed."
        exit 1
    fi
    echo "Running $TARGET"
    ./"$TARGET"
    exit $?
fi

# Fall back to g++
if command -v g++ &> /dev/null; then
    echo "Building with g++ $TRACE_FLAG"
    g++ -std=c++20 -O2 -Wall -Wextra -pedantic -I"$INC_DIR" $TRACE_FLAG "$SRC_DIR/main.cpp" -o "$TARGET"
    if [[ $? -ne 0 ]]; then
        echo "Build failed."
        exit 1
    fi
    echo "Running $TARGET"
    ./"$TARGET"
    exit $?
fi

echo "No C++ compiler found (clang++ or g++). Please install Xcode Command Line Tools or GCC."
exit 1