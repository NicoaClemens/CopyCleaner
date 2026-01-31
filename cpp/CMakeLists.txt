cmake_minimum_required(VERSION 3.20)
project(CopyCleaner VERSION 1.0 LANGUAGES CXX)

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/ast.cpp
    src/lexer.cpp
    src/parser.cpp
    src/runtime.cpp
    src/utils/runtime_utils.cpp
    src/main.cpp
)

# Header files (for IDE organization)
set(HEADERS
    include/ast_common.hpp
    include/ast.h
    include/errors.hpp
    include/lexer.h
    include/parser.h
    include/result.hpp
    include/runtime_value.h
    include/runtime.h
    include/utils/runtime_utils.h
    include/utils/types_utils.hpp
    include/utils/utils.hpp
    include/utils/variant_utils.hpp
)

# Create executable
add_executable(copycleaner ${SOURCES} ${HEADERS})

# Compiler warnings
if(MSVC)
    target_compile_options(copycleaner PRIVATE /W4)
else()
    target_compile_options(copycleaner PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Install target
install(TARGETS copycleaner DESTINATION bin)
