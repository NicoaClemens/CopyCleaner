cmake_minimum_required(VERSION 3.20)
project(CopyCleaner VERSION 1.0 LANGUAGES CXX)

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# SRC
file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
)

# Create executable
add_executable(copycleaner ${SOURCES})

target_include_directories(copycleaner PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Platform-specific libraries

if(APPLE)
    target_link_libraries(copycleaner PRIVATE "-framework CoreFoundation")
endif()

# Compiler warnings
if(MSVC)
    target_compile_options(copycleaner PRIVATE /W4)
else()
    target_compile_options(copycleaner PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Install target
install(TARGETS copycleaner DESTINATION bin)
