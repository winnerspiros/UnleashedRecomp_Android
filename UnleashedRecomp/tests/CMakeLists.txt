cmake_minimum_required(VERSION 3.20)
project(UnleashedRecompTests)

set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)

# Add C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(
    ${PROJECT_ROOT}
    ${PROJECT_ROOT}/UnleashedRecomp
    ${PROJECT_ROOT}/UnleashedRecomp/tests
    ${PROJECT_ROOT}/tools/XenonRecomp/XenonUtils
    ${PROJECT_ROOT}/thirdparty/unordered_dense/include
    ${PROJECT_ROOT}/tools/XenonRecomp/thirdparty/xxHash
    ${PROJECT_ROOT}/thirdparty/SDL/include
    ${PROJECT_ROOT}/thirdparty/fmt/include
    ${PROJECT_ROOT}/thirdparty/tomlplusplus/include
    ${PROJECT_ROOT}/thirdparty/imgui
)

# test_memory
add_executable(test_memory test_memory.cpp)
add_test(NAME MemoryTest COMMAND test_memory)

# test_config
add_executable(test_config test_config.cpp)
target_link_libraries(test_config PRIVATE fmt::fmt tomlplusplus::tomlplusplus SDL2::SDL2-static)
add_test(NAME ConfigTest COMMAND test_config)

# test_directory_file_system
add_executable(test_directory_file_system test_directory_file_system.cpp)
add_test(NAME DirectoryFileSystemTest COMMAND test_directory_file_system)

# test_mutex
add_executable(test_mutex test_mutex.cpp)
find_package(Threads REQUIRED)
target_link_libraries(test_mutex PRIVATE Threads::Threads)
add_test(NAME MutexTest COMMAND test_mutex)

# test_os_process
add_executable(test_os_process test_os_process.cpp)
if(UNIX)
    target_sources(test_os_process PRIVATE ${PROJECT_ROOT}/UnleashedRecomp/os/android/process_android.cpp)
endif()
add_test(NAME OSProcessTest COMMAND test_os_process)

# test_text_wrap
add_executable(test_text_wrap test_text_wrap.cpp)
target_sources(test_text_wrap PRIVATE
    ${PROJECT_ROOT}/UnleashedRecomp/ui/text_wrap_utils.cpp
    ${PROJECT_ROOT}/thirdparty/imgui/imgui.cpp
    ${PROJECT_ROOT}/thirdparty/imgui/imgui_draw.cpp
    ${PROJECT_ROOT}/thirdparty/imgui/imgui_tables.cpp
    ${PROJECT_ROOT}/thirdparty/imgui/imgui_widgets.cpp
    ${PROJECT_ROOT}/thirdparty/imgui/imgui_demo.cpp
)
add_test(NAME TextWrapTest COMMAND test_text_wrap)

# benchmark_iso_parsing
add_executable(benchmark_iso_parsing benchmark_iso_parsing.cpp
    ${PROJECT_ROOT}/UnleashedRecomp/install/iso_file_system.cpp
    ${PROJECT_ROOT}/tools/XenonRecomp/XenonUtils/memory_mapped_file.cpp
)

# test_iso_file_system
add_executable(test_iso_file_system test_iso_file_system.cpp
    ${PROJECT_ROOT}/UnleashedRecomp/install/iso_file_system.cpp
    ${PROJECT_ROOT}/tools/XenonRecomp/XenonUtils/memory_mapped_file.cpp
)
add_test(NAME ISOFileSystemTest COMMAND test_iso_file_system)

# test_xex_security
add_executable(test_xex_security test_xex_security.cpp)
target_include_directories(test_xex_security PRIVATE ${PROJECT_ROOT}/UnleashedRecomp/tests/mock)
add_test(NAME XexSecurityTest COMMAND test_xex_security)

# benchmark_buffer_upload
add_executable(benchmark_buffer_upload benchmark_buffer_upload.cpp)

# test_string_utils
add_executable(test_string_utils test_string_utils.cpp)
target_sources(test_string_utils PRIVATE ${PROJECT_ROOT}/UnleashedRecomp/ui/string_utils.cpp)
add_test(NAME StringUtilsTest COMMAND test_string_utils)

# test_ruby_utils
add_executable(test_ruby_utils test_ruby_utils.cpp)
target_sources(test_ruby_utils PRIVATE ${PROJECT_ROOT}/UnleashedRecomp/ui/ruby_utils.cpp)
add_test(NAME RubyUtilsTest COMMAND test_ruby_utils)

# test_string_holder
add_executable(test_string_holder test_string_holder.cpp)
add_test(NAME StringHolderTest COMMAND test_string_holder)

# benchmark_mod_loader_optimization
add_executable(benchmark_mod_loader_optimization benchmark_mod_loader_optimization.cpp)

# test_heap
add_executable(test_heap test_heap.cpp ${PROJECT_ROOT}/thirdparty/o1heap/o1heap.c)
target_include_directories(test_heap PRIVATE ${PROJECT_ROOT}/thirdparty/o1heap)
target_link_libraries(test_heap PRIVATE Threads::Threads)
add_test(NAME HeapTest COMMAND test_heap)

# test_freelist
add_executable(test_freelist test_freelist.cpp)
add_test(NAME FreeListTest COMMAND test_freelist)

# test_achievement_manager
add_executable(test_achievement_manager test_achievement_manager.cpp
    ${PROJECT_ROOT}/UnleashedRecomp/user/achievement_manager.cpp
    ${PROJECT_ROOT}/UnleashedRecomp/user/achievement_data.cpp
)

target_include_directories(test_achievement_manager PRIVATE
    ${PROJECT_ROOT}/UnleashedRecomp/tests/mock
    ${PROJECT_ROOT}
    ${PROJECT_ROOT}/UnleashedRecomp
)

target_compile_features(test_achievement_manager PRIVATE cxx_std_20)

add_test(NAME AchievementManagerTest COMMAND test_achievement_manager)

# test_xam
add_executable(test_xam test_xam.cpp
    ${PROJECT_ROOT}/UnleashedRecomp/kernel/xam.cpp
    ${PROJECT_ROOT}/UnleashedRecomp/tests/mock/xam_dependencies.cpp
)

target_include_directories(test_xam BEFORE PRIVATE
    ${PROJECT_ROOT}/UnleashedRecomp/tests/mock
)

target_include_directories(test_xam PRIVATE
    ${PROJECT_ROOT}/UnleashedRecomp
    ${PROJECT_ROOT}/tools/XenonRecomp/XenonUtils
)

target_link_libraries(test_xam PRIVATE Threads::Threads)

add_test(NAME XamTest COMMAND test_xam)
