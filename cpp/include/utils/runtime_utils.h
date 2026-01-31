// runtime_utils.h
#pragma once

#include "types_utils.hpp"
#include <optional>
#include "result.hpp"

namespace runtime_utils {

std::optional<double> to_f64(const RuntimeValue &v);

Result<RuntimeValue> numeric_add(const RuntimeValue &l, const RuntimeValue &r);
Result<RuntimeValue> numeric_sub(const RuntimeValue &l, const RuntimeValue &r);
Result<RuntimeValue> numeric_mul(const RuntimeValue &l, const RuntimeValue &r);
Result<RuntimeValue> numeric_div(const RuntimeValue &l, const RuntimeValue &r);
Result<RuntimeValue> numeric_pow(const RuntimeValue &l, const RuntimeValue &r);

Result<RuntimeValue> bool_and(const RuntimeValue &l, const RuntimeValue &r);
Result<RuntimeValue> bool_or(const RuntimeValue &l, const RuntimeValue &r);

Result<RuntimeValue> concat(const RuntimeValue &l, const RuntimeValue &r);

Result<RuntimeValue> compare_gt(const RuntimeValue &l, const RuntimeValue &r);
Result<RuntimeValue> compare_lt(const RuntimeValue &l, const RuntimeValue &r);
Result<RuntimeValue> compare_ge(const RuntimeValue &l, const RuntimeValue &r);
Result<RuntimeValue> compare_le(const RuntimeValue &l, const RuntimeValue &r);

}
