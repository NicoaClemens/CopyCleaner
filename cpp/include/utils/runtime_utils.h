// runtime_utils.h
// Declares: to_f64, numeric_add, numeric_sub, numeric_mul, numeric_div, numeric_pow, bool_and,
// bool_or, concat, compare_gt, compare_lt, compare_ge, compare_le

#pragma once

#include <optional>

#include "result.hpp"
#include "types_utils.hpp"

namespace runtime_utils {

std::optional<double> to_f64(const RuntimeValue& v);

Result<RuntimeValue> numeric_add(const RuntimeValue& l, const RuntimeValue& r);
Result<RuntimeValue> numeric_sub(const RuntimeValue& l, const RuntimeValue& r);
Result<RuntimeValue> numeric_mul(const RuntimeValue& l, const RuntimeValue& r);
Result<RuntimeValue> numeric_div(const RuntimeValue& l, const RuntimeValue& r);
Result<RuntimeValue> numeric_pow(const RuntimeValue& l, const RuntimeValue& r);

Result<RuntimeValue> bool_and(const RuntimeValue& l, const RuntimeValue& r);
Result<RuntimeValue> bool_or(const RuntimeValue& l, const RuntimeValue& r);

Result<RuntimeValue> concat(const RuntimeValue& l, const RuntimeValue& r);

Result<RuntimeValue> compare_gt(const RuntimeValue& l, const RuntimeValue& r);
Result<RuntimeValue> compare_lt(const RuntimeValue& l, const RuntimeValue& r);
Result<RuntimeValue> compare_ge(const RuntimeValue& l, const RuntimeValue& r);
Result<RuntimeValue> compare_le(const RuntimeValue& l, const RuntimeValue& r);

}  // namespace runtime_utils
