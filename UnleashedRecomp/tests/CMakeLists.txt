add_executable(freelist_test test_freelist.cpp)
target_compile_features(freelist_test PRIVATE cxx_std_20)
add_test(NAME freelist_test COMMAND freelist_test)
