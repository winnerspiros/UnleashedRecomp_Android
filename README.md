<p align="center">
    <img src="https://raw.githubusercontent.com/hedge-dev/UnleashedRecompResources/refs/heads/main/images/logo/Logo.png" width="512"/>
</p>

---

# Unleashed Recompiled: Android Edition

This is an **unofficial Android port** of *Sonic Unleashed Recompiled* (originally for PC).  
It is based on the static recompilation of the Xbox 360 version of Sonic Unleashed.

**Important:** This project does **not include any game assets**. You must provide them from a legally acquired copy.

## Features

- Full Android support (ARM64 only)  
- High-performance rendering via **Vulkan**  
- Touch and controller input  
- Mod support via Hedge Mod Manager (limited on Android)  
- High-resolution and ultrawide support (device dependent)  
- Core gameplay logic ported from PC recompilation  

## Android Device Requirements

- **CPU:** ARMv8-A 64-bit (ARM64) required  
- **RAM:** 4 GB minimum, 6+ GB recommended  
- **GPU:** Vulkan 1.1 support required  
- **Storage:** 6–10 GB for game assets  
- **OS:** Android 10+  

> Devices without Vulkan support are not compatible. Older ARM32 devices are not supported.

## How to Install / Build APK

### Prebuilt APK

If a prebuilt APK is provided in the releases, install it directly on your device.

### Build from Source using GitHub Actions

1. Fork or clone this repository.
2. Place your legally acquired Xbox 360 game files in the proper asset directories.
3. Push a branch to your fork and trigger the `release.yml` workflow.
4. The workflow will build the APK automatically.
5. Download the generated APK from the workflow artifacts.
6. Install the APK on your Android device.

### Manual Build (Optional)

1. Install Android Studio with NDK and CMake.  
2. Configure the Gradle build with your assets path.  
3. Build the APK using:
   ```bash
   ./gradlew assembleRelease
