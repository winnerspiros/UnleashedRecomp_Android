#include <stdafx.h>
#include <SDL.h>
#include <hid/hid.h>
#include <ui/game_window.h>
#include <cpu/guest_thread.h>
#include <config.h>
#include <user/paths.h>
#include <kernel/heap.h>
// #include <kernel/memory.h> // Removed to avoid redefinition conflict with xdm.h
#include <kernel/xdm.h>
#include <mod/mod_loader.h>

// Mock Global Heap and Memory
Heap g_userHeap;
Memory g_memory;

// Mock Memory Functions
void* MmGetHostAddress(uint32_t ptr) { return g_memory.Translate(ptr); }

// Mock Kernel Object Functions
uint32_t GetKernelHandle(KernelObject* obj) { return (uint32_t)(uintptr_t)obj; }
void DestroyKernelObject(KernelObject* obj) { /* Mock */ }
void DestroyKernelObject(uint32_t handle) { /* Mock */ }
bool IsKernelObject(uint32_t handle) { return true; }
bool IsKernelObject(void* obj) { return true; }
bool IsInvalidKernelObject(void* obj) { return false; }

// Mock HID Functions
namespace hid {
    uint16_t g_prohibitedButtons = 0;
    bool g_isLeftStickProhibited = false;
    bool g_isRightStickProhibited = false;

    uint32_t GetCapabilities(uint32_t userIndex, XAMINPUT_CAPABILITIES* caps) { return ERROR_SUCCESS; }
    bool IsInputAllowed() { return true; }
    uint32_t GetState(uint32_t userIndex, XAMINPUT_STATE* state) { return ERROR_SUCCESS; }
    uint32_t SetState(uint32_t userIndex, XAMINPUT_VIBRATION* vibration) { return ERROR_SUCCESS; }
    bool IsInputDeviceController() { return true; }
}

// Mock SDL
const Uint8* SDL_GetKeyboardState(int* numkeys) {
    static Uint8 keys[SDL_NUM_SCANCODES] = {0};
    if (numkeys) *numkeys = SDL_NUM_SCANCODES;
    return keys;
}

// Mock GameWindow
bool GameWindow::s_isFocused = true;

// Mock GuestThread
uint32_t GuestThread::GetCurrentThreadId() { return 1; }
void GuestThread::SetLastError(uint32_t error) { /* Mock */ }

// Mock Config
namespace Config {
    int Key_LeftStickUp = 0;
    int Key_LeftStickDown = 0;
    int Key_LeftStickLeft = 0;
    int Key_LeftStickRight = 0;
    int Key_RightStickUp = 0;
    int Key_RightStickDown = 0;
    int Key_RightStickLeft = 0;
    int Key_RightStickRight = 0;
    int Key_LeftTrigger = 0;
    int Key_RightTrigger = 0;
    int Key_DPadUp = 0;
    int Key_DPadDown = 0;
    int Key_DPadLeft = 0;
    int Key_DPadRight = 0;
    int Key_Start = 0;
    int Key_Back = 0;
    int Key_LeftBumper = 0;
    int Key_RightBumper = 0;
    int Key_A = 0;
    int Key_B = 0;
    int Key_X = 0;
    int Key_Y = 0;
    bool Vibration = false;
}

// Mock Paths Variables
// ModLoader::s_saveFilePath is static inline in header.
std::filesystem::path g_executableRoot = ".";

// GetUserPath removed as it is inline in mock/user/paths.h

// Mock Heap Implementation
void Heap::Init() {}
void* Heap::Alloc(size_t size) { return malloc(size); }
void* Heap::AllocPhysical(size_t size, size_t alignment) { return malloc(size); }
void Heap::Free(void* ptr) { free(ptr); }
size_t Heap::Size(void* ptr) { return 0; }

// Mock Memory Implementation
Memory::Memory() : base((uint8_t*)malloc(PPC_MEMORY_SIZE)) {}
