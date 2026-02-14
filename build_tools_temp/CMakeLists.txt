cmake_minimum_required(VERSION 3.20)
project(HostTools)

set(UNLEASHED_RECOMP_TOOLS_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../tools")
set(CMAKE_CXX_STANDARD 20)

# Enable MSVC extensions for GCC/Clang (try to support existing code)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-fms-extensions)
endif()

# Define TOOLS_BUILD to allow hiding problematic code in headers
add_compile_definitions(TOOLS_BUILD)

# Add tools subdirectory
add_subdirectory("${UNLEASHED_RECOMP_TOOLS_ROOT}" tools)
