#!/bin/bash

# Check if clang-format is installed
if ! command -v clang-format &> /dev/null; then
    echo "Error: clang-format is not installed."
    echo "Please install clang-format and then set up the build environment again."
    exit 1
fi

# If clang-format is installed, proceed with the build setup
echo "clang-format is installed. Proceeding..."
builddir="$1"

# Set default build directory if not provided
if [ -z "$builddir" ]; then
    builddir="build"
fi

# Check if Meson build directory exists
if [ ! -d "$builddir" ]; then
    echo "Build directory '$builddir' does not exist. Please run 'meson setup $builddir' first."
    exit 1
fi

# Run ninja to format code
ninja -C "$builddir" clang-format
