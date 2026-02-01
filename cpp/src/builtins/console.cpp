// console.cpp
// Implements builtins/console.h

#include "builtins/console.h"

#include <iostream>

namespace builtins {

Result<RuntimeValue> Console::print(const std::string& message) {
    // Print to stdout - does nothing if no console is available
    // (which is the expected behavior per the spec)
    std::cout << message << std::endl;

    RuntimeValue result;
    result.value = RuntimeValue::Null{};
    return ok(result);
}

}  // namespace builtins
