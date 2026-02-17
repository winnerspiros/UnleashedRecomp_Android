#!/bin/bash
set -e

# Build script for Android APK

# Check for private assets
if [ -d "private" ]; then
    echo "Copying private assets..."
    mkdir -p UnleashedRecompLib/private

    echo "Searching for default.xex..."
    find private -iname "default.xex" -exec cp {} UnleashedRecompLib/private/default.xex \;

    echo "Searching for default.xexp..."
    find private -iname "default.xexp" -exec cp {} UnleashedRecompLib/private/default.xexp \;

    echo "Searching for shader.ar..."
    find private -iname "shader.ar" -exec cp {} UnleashedRecompLib/private/shader.ar \;
else
    echo "Warning: 'private' directory not found. Build may fail or be incomplete."
fi

# Bootstrap vcpkg
echo "Bootstrapping vcpkg..."
./thirdparty/vcpkg/bootstrap-vcpkg.sh

# Apply NFD Android Patch
echo "Applying nativefiledialog-extended Android patch..."
cd thirdparty/nativefiledialog-extended
if ! grep -q "PLATFORM_ANDROID" CMakeLists.txt; then
    git apply ../../patches/nfd_android.patch
else
    echo "Patch already applied."
fi
cd ../..

# Apply SWA Patch
echo "Applying SWA patch..."
cd UnleashedRecomp/api
if grep -q "// virtual ~IParallelJob" Hedgehog/Universe/Thread/hhParallelJob.h; then
    git apply ../../patches/swa_virtual_funcs.patch
else
    echo "SWA patch already applied."
fi
cd ../..

# Build APK
echo "Building APK..."
cd android
./gradlew assembleDebug

echo "Build complete!"
echo "APK located at: android/app/build/outputs/apk/debug/app-debug.apk"
