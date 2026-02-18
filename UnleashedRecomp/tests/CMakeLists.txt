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

# test_mod_loader
add_executable(test_mod_loader test_mod_loader.cpp)

target_include_directories(test_mod_loader PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/UnleashedRecomp
    ${CMAKE_SOURCE_DIR}/thirdparty/unordered_dense/include
)

target_compile_features(test_mod_loader PRIVATE cxx_std_20)

target_link_libraries(test_mod_loader PRIVATE
    xxHash::xxhash
)

add_test(NAME ModLoaderTest COMMAND test_mod_loader)
