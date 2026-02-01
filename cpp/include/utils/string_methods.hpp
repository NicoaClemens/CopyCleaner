#pragma once

#include "../result.hpp"
#include "../runtime_value.h"
#include <vector>

namespace StringMethods {

// String length
Result<RuntimeValue> length(const std::vector<RuntimeValue>& args);

// String manipulation
Result<RuntimeValue> toUpper(const std::vector<RuntimeValue>& args);
Result<RuntimeValue> toLower(const std::vector<RuntimeValue>& args);
Result<RuntimeValue> trim(const std::vector<RuntimeValue>& args);
Result<RuntimeValue> substring(const std::vector<RuntimeValue>& args);
Result<RuntimeValue> replace(const std::vector<RuntimeValue>& args);

// String search/query
Result<RuntimeValue> contains(const std::vector<RuntimeValue>& args);
Result<RuntimeValue> startsWith(const std::vector<RuntimeValue>& args);
Result<RuntimeValue> endsWith(const std::vector<RuntimeValue>& args);
Result<RuntimeValue> indexOf(const std::vector<RuntimeValue>& args);

// String splitting
Result<RuntimeValue> split(const std::vector<RuntimeValue>& args);

// Match-related
Result<RuntimeValue> hasMatch(const std::vector<RuntimeValue>& args);
Result<RuntimeValue> replaceMatch(const std::vector<RuntimeValue>& args);

}  // namespace StringMethods
