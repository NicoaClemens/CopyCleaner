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
    src/utils/builtin_functions.cpp
    src/utils/string_methods.cpp
    src/utils/list_methods.cpp
    src/utils/regex_methods.cpp
    src/utils/method_dispatcher.cpp
    src/builtins/logger.cpp
    src/builtins/console.cpp
    src/builtins/clipboard.cpp
    src/builtins/alert.cpp
    src/main.cpp
)

# Header files
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
    include/utils/builtin_functions.h
    include/utils/types_utils.hpp
    include/utils/utils.hpp
    include/utils/variant_utils.hpp
    include/utils/string_methods.hpp
    include/utils/list_methods.hpp
    include/utils/regex_methods.hpp
    include/utils/method_dispatcher.hpp
    include/builtins/logger.h
    include/builtins/console.h
    include/builtins/clipboard.h
    include/builtins/alert.h
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
