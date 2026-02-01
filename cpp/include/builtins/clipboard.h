// clipboard.h
// Declares: Clipboard built-in functions

#pragma once

#include <string>

#include "result.hpp"
#include "runtime_value.h"

namespace builtins {

/// @brief Provides clipboard access functionality for the CopyCleaner interpreter
class Clipboard {
   public:
    /// @brief Checks if clipboard content is text
    /// @return Result containing true if clipboard contains text, false otherwise
    Result<RuntimeValue> is_text();

    /// @brief Reads clipboard content as text
    /// @return Result containing the clipboard text, or empty string if clipboard is binary
    Result<RuntimeValue> read();

    /// @brief Writes text to clipboard
    /// @param message The text to write to clipboard
    /// @return Result containing true on success, false on error (message too long, clipboard unavailable)
    Result<RuntimeValue> write(const std::string& message);
};

}  // namespace builtins
