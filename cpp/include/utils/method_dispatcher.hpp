#pragma once

#include <string>
#include <vector>

#include "../result.hpp"
#include "../runtime_value.h"

namespace MethodDispatcher {

// Dispatch method calls to appropriate handler
Result<RuntimeValue> dispatchMethod(const std::string& methodName,
                                    const std::vector<RuntimeValue>& args);

}  // namespace MethodDispatcher
