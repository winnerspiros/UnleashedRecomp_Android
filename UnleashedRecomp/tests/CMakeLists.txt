add_executable(misc_test test_misc.cpp)

# Include the parent directory to find "misc_impl.h"
# Include the current directory to find "doctest.h"
target_include_directories(misc_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(misc_test PUBLIC cxx_std_20)

add_test(NAME misc_test COMMAND misc_test)
