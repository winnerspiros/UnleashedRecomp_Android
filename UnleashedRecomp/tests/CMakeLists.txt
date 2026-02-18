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

# test_file_system
add_executable(test_file_system test_file_system.cpp)

target_include_directories(test_file_system PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/UnleashedRecomp
)

target_compile_features(test_file_system PRIVATE cxx_std_20)

target_link_libraries(test_file_system PRIVATE
    fmt::fmt
    tomlplusplus::tomlplusplus
    SDL2::SDL2-static
    o1heap
    XenonUtils
)

add_test(NAME FileSystemTest COMMAND test_file_system)
