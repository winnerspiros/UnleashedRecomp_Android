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

# Apply XenonRecomp Patch
echo "Applying XenonRecomp fixes..."
cd tools/XenonRecomp
if ! grep -q "if (NOT ANDROID)" CMakeLists.txt; then
    echo "Applying patch via git apply..."
    if ! git apply ../../patches/xenon_recomp_fixes.patch; then
        echo "git apply failed, attempting to use patch..."
        patch -p1 < ../../patches/xenon_recomp_fixes.patch
    fi
else
    echo "XenonRecomp fixes already applied."
fi
cd ../..

# Apply XenosRecomp Patch
echo "Applying XenosRecomp fixes..."
cd tools/XenosRecomp
if ! grep -q "MATCHES \"GNU|Clang\"" XenosRecomp/CMakeLists.txt; then
    echo "Applying patch via git apply..."
    if ! git apply ../../patches/xenos_recomp_fixes.patch; then
        echo "git apply failed, attempting to use patch..."
        patch -p1 < ../../patches/xenos_recomp_fixes.patch
    fi
else
    echo "XenosRecomp fixes already applied."
fi
cd ../..

# Apply SDL Android Patch
echo "Applying SDL Android fixes..."
cd thirdparty/SDL
if ! grep -q "ASensorManager_getInstanceForPackage" src/sensor/android/SDL_androidsensor.c; then
    echo "Applying patch via git apply..."
    if ! git apply ../../patches/sdl_android_fixes.patch; then
        echo "git apply failed, attempting to use patch..."
        patch -p1 < ../../patches/sdl_android_fixes.patch
    fi
else
    echo "SDL Android fixes already applied."
fi
cd ../..

# Apply Hedgehog String Holder Patch
echo "Applying Hedgehog String Holder patch..."
cd UnleashedRecomp/api
if ! grep -q "std::memcpy" Hedgehog/Base/Type/detail/hhStringHolder.inl; then
    echo "Applying patch via git apply..."
    if ! git apply ../../patches/hh_string_holder_fix.patch; then
        echo "git apply failed, attempting to use patch..."
        patch -p1 < ../../patches/hh_string_holder_fix.patch
    fi
else
    echo "Hedgehog String Holder patch already applied."
fi
cd ../..

# Build APK
echo "Building APK..."
cd android
./gradlew assembleDebug

echo "Build complete!"
echo "APK located at: android/app/build/outputs/apk/debug/app-debug.apk"
