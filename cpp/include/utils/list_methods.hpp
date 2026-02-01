#pragma once

#include "../result.hpp"
#include "../runtime_value.h"
#include <vector>

namespace ListMethods {

// List length
Result<RuntimeValue> length(const std::vector<RuntimeValue>& args);

// List access
Result<RuntimeValue> get(const std::vector<RuntimeValue>& args);

// List modification
Result<RuntimeValue> push(const std::vector<RuntimeValue>& args);
Result<RuntimeValue> slice(const std::vector<RuntimeValue>& args);

// List search/query
Result<RuntimeValue> contains(const std::vector<RuntimeValue>& args);
Result<RuntimeValue> indexOf(const std::vector<RuntimeValue>& args);

}  // namespace ListMethods
