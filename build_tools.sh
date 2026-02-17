#!/bin/bash
set -e

# Make sure we are in the project root
cd "$(dirname "$0")"

# Define absolute path for output binaries
OUTPUT_BIN_DIR="$(pwd)/build_tools/bin"
mkdir -p "$OUTPUT_BIN_DIR"

# Apply XenonRecomp patch
if [ -f "patches/xenon_recomp_fixes.patch" ]; then
    echo "Applying XenonRecomp fixes..."
    cd tools/XenonRecomp
    git apply ../../patches/xenon_recomp_fixes.patch || echo "Warning: Failed to apply patch or already applied."
    cd ../..
fi

echo "=== Building GCC compatible tools ==="
rm -rf build_tools/build_gcc
mkdir -p build_tools/build_gcc
cd build_tools/build_gcc

export CC=gcc
export CXX=g++

# Configure
cmake ../.. -DCMAKE_BUILD_TYPE=Release -DBUILD_TOOLS_ONLY=ON -DCMAKE_CXX_FLAGS="-fms-extensions"

# Build
cmake --build . --target file_to_c fshasher x_decompress bc_diff --parallel $(nproc)

# Install function using absolute path
copy_tool() {
    local tool_name=$1
    local tool_path=$(find . -name "$tool_name" -type f -executable | head -n 1)
    if [ -n "$tool_path" ]; then
        cp "$tool_path" "$OUTPUT_BIN_DIR/"
        echo "Copied $tool_name to $OUTPUT_BIN_DIR"
    else
        echo "Error: Could not find executable for $tool_name"
        exit 1
    fi
}

copy_tool "file_to_c"
copy_tool "fshasher"
copy_tool "x_decompress"
copy_tool "bc_diff"

cd ../..

echo "=== Building Clang compatible tools (XenonRecomp/XenosRecomp) ==="
# XenonUtils requires MSVC extensions which Clang supports but GCC does not.
# However, Clang C compiler crashed on zstd, so we use GCC for C code.
# And file_to_c failed with Clang++, so we built it with G++ above.

rm -rf build_tools/build_clang
mkdir -p build_tools/build_clang
cd build_tools/build_clang

export CC=gcc
export CXX=g++

# Configure
cmake ../.. -DCMAKE_BUILD_TYPE=Release -DBUILD_TOOLS_ONLY=ON

# Build
cmake --build . --target XenonRecomp XenosRecomp --parallel $(nproc)

# Install
copy_tool "XenonRecomp"
copy_tool "XenosRecomp"

cd ../..

# Copy libdxcompiler.so
cp tools/XenosRecomp/thirdparty/dxc-bin/lib/x64/libdxcompiler.so "$OUTPUT_BIN_DIR/"
echo "Copied libdxcompiler.so"

echo "Build tools ready in $OUTPUT_BIN_DIR"
ls -l "$OUTPUT_BIN_DIR"
