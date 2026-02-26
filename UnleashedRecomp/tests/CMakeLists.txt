add_executable(test_memory test_memory.cpp)

# Include directories
target_include_directories(test_memory PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/UnleashedRecomp
)

# Set C++ standard
target_compile_features(test_memory PRIVATE cxx_std_20)

add_test(NAME MemoryTest COMMAND test_memory)

# test_config
add_executable(test_config test_config.cpp)

target_include_directories(test_config PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/UnleashedRecomp
)

target_compile_features(test_config PRIVATE cxx_std_20)

target_link_libraries(test_config PRIVATE
    fmt::fmt
    tomlplusplus::tomlplusplus
    SDL2::SDL2-static
)

add_test(NAME ConfigTest COMMAND test_config)

# test_directory_file_system
add_executable(test_directory_file_system test_directory_file_system.cpp)

target_include_directories(test_directory_file_system PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/UnleashedRecomp
)

target_compile_features(test_directory_file_system PRIVATE cxx_std_20)

add_test(NAME DirectoryFileSystemTest COMMAND test_directory_file_system)

# test_mutex
add_executable(test_mutex test_mutex.cpp)

target_include_directories(test_mutex PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/UnleashedRecomp
)

target_compile_features(test_mutex PRIVATE cxx_std_20)

# On Linux, we need to link against pthreads implicitly, usually covered by compiler or standard lib
# But if threads are used, sometimes we need to link against Threads::Threads.
find_package(Threads REQUIRED)
target_link_libraries(test_mutex PRIVATE Threads::Threads)

add_test(NAME MutexTest COMMAND test_mutex)

# test_os_process
add_executable(test_os_process test_os_process.cpp)

# Compile process_android.cpp if on UNIX (Linux/Android/macOS)
if(UNIX)
    target_sources(test_os_process PRIVATE ${CMAKE_SOURCE_DIR}/UnleashedRecomp/os/android/process_android.cpp)
endif()

target_include_directories(test_os_process PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/UnleashedRecomp
)

target_compile_features(test_os_process PRIVATE cxx_std_20)

add_test(NAME OSProcessTest COMMAND test_os_process)
# test_text_wrap
add_executable(test_text_wrap test_text_wrap.cpp)

target_sources(test_text_wrap PRIVATE
    ${CMAKE_SOURCE_DIR}/UnleashedRecomp/ui/text_wrap_utils.cpp
    ${CMAKE_SOURCE_DIR}/thirdparty/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/thirdparty/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/thirdparty/imgui/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/thirdparty/imgui/imgui_widgets.cpp
    ${CMAKE_SOURCE_DIR}/thirdparty/imgui/imgui_demo.cpp
)

target_include_directories(test_text_wrap PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/UnleashedRecomp
    ${CMAKE_SOURCE_DIR}/thirdparty/imgui
)

target_compile_features(test_text_wrap PRIVATE cxx_std_20)

# ImGui might need SDL if configured so? ImGui configuration is in imconfig.h which is in imgui folder.
# Let's assume default config.

add_test(NAME TextWrapTest COMMAND test_text_wrap)

# benchmark_iso_parsing
add_executable(benchmark_iso_parsing benchmark_iso_parsing.cpp
    ${CMAKE_SOURCE_DIR}/UnleashedRecomp/install/iso_file_system.cpp
    ${CMAKE_SOURCE_DIR}/tools/XenonRecomp/XenonUtils/memory_mapped_file.cpp
)

target_include_directories(benchmark_iso_parsing PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/UnleashedRecomp
    ${CMAKE_SOURCE_DIR}/tools/XenonRecomp/XenonUtils
    ${CMAKE_SOURCE_DIR}/thirdparty/unordered_dense/include
)

target_compile_features(benchmark_iso_parsing PRIVATE cxx_std_20)

# test_iso_file_system
add_executable(test_iso_file_system test_iso_file_system.cpp
    ${CMAKE_SOURCE_DIR}/UnleashedRecomp/install/iso_file_system.cpp
    ${CMAKE_SOURCE_DIR}/tools/XenonRecomp/XenonUtils/memory_mapped_file.cpp
)

target_include_directories(test_iso_file_system PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/UnleashedRecomp
    ${CMAKE_SOURCE_DIR}/tools/XenonRecomp/XenonUtils
    ${CMAKE_SOURCE_DIR}/thirdparty/unordered_dense/include
)

target_compile_features(test_iso_file_system PRIVATE cxx_std_20)

add_test(NAME ISOFileSystemTest COMMAND test_iso_file_system)

# test_xex_security
add_executable(test_xex_security test_xex_security.cpp)

target_include_directories(test_xex_security PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/UnleashedRecomp
    ${CMAKE_SOURCE_DIR}/UnleashedRecomp/tests/mock
)

target_compile_features(test_xex_security PRIVATE cxx_std_20)

add_test(NAME XexSecurityTest COMMAND test_xex_security)

# benchmark_buffer_upload
add_executable(benchmark_buffer_upload benchmark_buffer_upload.cpp)

target_include_directories(benchmark_buffer_upload PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/UnleashedRecomp
    ${CMAKE_SOURCE_DIR}/tools/XenonRecomp/XenonUtils
)

target_compile_features(benchmark_buffer_upload PRIVATE cxx_std_20)

# test_string_utils
add_executable(test_string_utils test_string_utils.cpp)

target_include_directories(test_string_utils PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/UnleashedRecomp
)

target_compile_features(test_string_utils PRIVATE cxx_std_20)

target_sources(test_string_utils PRIVATE ${CMAKE_SOURCE_DIR}/UnleashedRecomp/ui/string_utils.cpp)

add_test(NAME StringUtilsTest COMMAND test_string_utils)

# test_ruby_utils
add_executable(test_ruby_utils test_ruby_utils.cpp)

target_include_directories(test_ruby_utils PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/UnleashedRecomp
)

target_compile_features(test_ruby_utils PRIVATE cxx_std_20)

target_sources(test_ruby_utils PRIVATE ${CMAKE_SOURCE_DIR}/UnleashedRecomp/ui/ruby_utils.cpp)

add_test(NAME RubyUtilsTest COMMAND test_ruby_utils)

# test_string_holder
add_executable(test_string_holder test_string_holder.cpp)

target_include_directories(test_string_holder PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/UnleashedRecomp
)

target_compile_features(test_string_holder PRIVATE cxx_std_20)

add_test(NAME StringHolderTest COMMAND test_string_holder)

# benchmark_mod_loader_optimization
add_executable(benchmark_mod_loader_optimization benchmark_mod_loader_optimization.cpp)

target_include_directories(benchmark_mod_loader_optimization PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/UnleashedRecomp
    ${CMAKE_SOURCE_DIR}/thirdparty/unordered_dense/include
)

target_compile_features(benchmark_mod_loader_optimization PRIVATE cxx_std_20)

# test_heap
add_executable(test_heap test_heap.cpp ${CMAKE_SOURCE_DIR}/thirdparty/o1heap/o1heap.c)

target_include_directories(test_heap PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/UnleashedRecomp
    ${CMAKE_SOURCE_DIR}/thirdparty/o1heap
)

target_compile_features(test_heap PRIVATE cxx_std_20)

target_link_libraries(test_heap PRIVATE Threads::Threads)

add_test(NAME HeapTest COMMAND test_heap)

# test_update_unit
add_executable(test_update_unit test_update_unit.cpp)

target_include_directories(test_update_unit PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/UnleashedRecomp
    ${CMAKE_SOURCE_DIR}/UnleashedRecomp/api
    ${CMAKE_SOURCE_DIR}/tools/XenonRecomp/XenonUtils
    ${CMAKE_SOURCE_DIR}/tools/XenonRecomp/thirdparty/simde
    ${CMAKE_SOURCE_DIR}/tools/XenonRecomp/thirdparty/fmt/include
    ${CMAKE_SOURCE_DIR}/tools/XenonRecomp/thirdparty/xxHash
    ${CMAKE_SOURCE_DIR}/thirdparty/unordered_dense/include
)

target_compile_features(test_update_unit PRIVATE cxx_std_20)
target_link_libraries(test_update_unit PRIVATE Threads::Threads)

add_test(NAME UpdateUnitTest COMMAND test_update_unit)
