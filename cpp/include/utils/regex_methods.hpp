#pragma once

#include "../result.hpp"
#include "../runtime_value.h"
#include <vector>

namespace RegexMethods {

// Regex methods
Result<RuntimeValue> getAll(const std::vector<RuntimeValue>& args);

}  // namespace RegexMethods
