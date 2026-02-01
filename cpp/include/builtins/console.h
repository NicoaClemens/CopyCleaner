// console.h
// Declares: Console built-in functions

#pragma once

#include <string>

#include "result.hpp"
#include "runtime_value.h"

namespace builtins {

/// @brief Provides console output functionality
class Console {
   public:
    /// @brief Prints a message to standard output
    /// @param message The message to print (already converted to string)
    /// @return Result containing null (always succeeds, even if no console is available)
    Result<RuntimeValue> print(const std::string& message);
};

}  // namespace builtins
