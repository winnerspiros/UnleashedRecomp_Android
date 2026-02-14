#!/bin/bash
set -e

# Build script for Android APK

# Check for private assets
if [ -d "private" ]; then
    echo "Copying private assets..."
    cp -r private/* UnleashedRecompLib/private/
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

# Build tools
./build_tools.sh

# Build APK
echo "Building APK..."
cd android
./gradlew assembleDebug

echo "Build complete!"
echo "APK located at: android/app/build/outputs/apk/debug/app-debug.apk"
