project(UnleashedRecompTests)

add_executable(test_xxhash test_xxhash.cpp)

target_link_libraries(test_xxhash PRIVATE
    xxHash::xxhash
)

# Include unordered_dense directory directly as it's not exposed as a target in the main build
target_include_directories(test_xxhash PRIVATE
    ${CMAKE_SOURCE_DIR}/thirdparty/unordered_dense/include
)

# We need to include the directory where xxHashMap.h resides
target_include_directories(test_xxhash PRIVATE
    ${CMAKE_SOURCE_DIR}/UnleashedRecomp
)

add_test(NAME xxHashTest COMMAND test_xxhash)
