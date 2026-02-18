
add_executable(test_memory test_memory.cpp)

# Include directories
target_include_directories(test_memory PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/UnleashedRecomp
)

# Set C++ standard
target_compile_features(test_memory PRIVATE cxx_std_20)

add_test(NAME MemoryTest COMMAND test_memory)

add_executable(test_arg_translator test_arg_translator.cpp)

# Link against XenonUtils for PPCContext and xbox.h
target_link_libraries(test_arg_translator PRIVATE XenonUtils)

# Include directories
target_include_directories(test_arg_translator PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/UnleashedRecomp
)

# Set C++ standard
target_compile_features(test_arg_translator PRIVATE cxx_std_20)

add_test(NAME ArgTranslatorTest COMMAND test_arg_translator)
