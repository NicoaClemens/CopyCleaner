// builtin_functions.h
// Declares: call_builtin

#pragma once

#include <string>
#include <vector>

#include "result.hpp"
#include "runtime_value.h"

namespace builtins {
    class Logger;
    class Console;
    class Clipboard;
    class Alert;
}

class Interpreter;

/// @brief Built-in function dispatcher for CopyCleaner runtime
namespace builtin_functions {

/// @brief Dispatches and executes a built-in function by name
/// @param name The name of the built-in function to call
/// @param args The evaluated arguments to pass to the function
/// @param logger Reference to the logger instance
/// @param console Reference to the console instance
/// @param clipboard Reference to the clipboard instance
/// @param alert Reference to the alert instance
/// @param interp Pointer to the interpreter (unused currently, reserved for future use)
/// @return Result containing the return value or an error
Result<RuntimeValue> call_builtin(const std::string& name, const std::vector<RuntimeValue>& args,
                                  builtins::Logger& logger, builtins::Console& console,
                                  builtins::Clipboard& clipboard, builtins::Alert& alert,
                                  Interpreter* interp);

}  // namespace builtin_functions
