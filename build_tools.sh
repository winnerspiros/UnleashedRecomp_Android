#!/bin/bash
set -e

echo "Building tools for host..."

# Apply patches to fix compilation on Linux and disable Android executable targets
echo "Applying patches..."

cd tools/XenonRecomp
# Check if patch is already applied (simple check for TOOLS_BUILD in xbox.h)
if ! grep -q "TOOLS_BUILD" XenonUtils/xbox.h; then
    echo "Applying xenon_recomp.patch..."
    git apply ../../patches/xenon_recomp.patch
else
    echo "xenon_recomp.patch already applied."
fi
cd ../..

cd tools/XenosRecomp
# Check if patch is already applied (simple check for Int4 struct in shader_recompiler.cpp)
if ! grep -q "struct Int4" XenosRecomp/shader_recompiler.cpp; then
    echo "Applying xenos_recomp.patch..."
    git apply ../../patches/xenos_recomp.patch
else
    echo "xenos_recomp.patch already applied."
fi
cd ../..

# Clean up previous build directory
rm -rf build_tools_temp
mkdir -p build_tools_temp
mkdir -p build_tools/bin

# Create a minimal CMakeLists.txt to build only the tools
cat <<EOF > build_tools_temp/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(HostTools)

set(UNLEASHED_RECOMP_TOOLS_ROOT "\${CMAKE_CURRENT_SOURCE_DIR}/../tools")
set(CMAKE_CXX_STANDARD 20)

# Enable MSVC extensions for GCC/Clang (try to support existing code)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-fms-extensions)
endif()

# Define TOOLS_BUILD to allow hiding problematic code in headers
add_compile_definitions(TOOLS_BUILD)

# Add tools subdirectory
add_subdirectory("\${UNLEASHED_RECOMP_TOOLS_ROOT}" tools)
EOF

cd build_tools_temp

# Configure CMake for host
# Use -Wno-dev to suppress warnings about policy CMP0077 etc.
cmake . -DCMAKE_BUILD_TYPE=Release -Wno-dev

# Build required tools
echo "Compiling XenonRecomp, XenosRecomp, x_decompress, file_to_c..."
cmake --build . --target XenonRecomp XenosRecomp x_decompress file_to_c --parallel $(nproc)

echo "Copying binaries..."
# The build output structure will be under 'tools' directory in build_tools_temp

cp tools/XenonRecomp/XenonRecomp/XenonRecomp ../build_tools/bin/
cp tools/XenosRecomp/XenosRecomp/XenosRecomp ../build_tools/bin/
cp tools/x_decompress/x_decompress ../build_tools/bin/
cp tools/file_to_c/file_to_c ../build_tools/bin/

cd ..

echo "Copying libraries..."
# Copy libdxcompiler.so and libdxil.so
cp tools/XenosRecomp/thirdparty/dxc-bin/lib/x64/libdxcompiler.so build_tools/bin/
cp tools/XenosRecomp/thirdparty/dxc-bin/lib/x64/libdxil.so build_tools/bin/

echo "Tools built successfully!"
ls -l build_tools/bin/
