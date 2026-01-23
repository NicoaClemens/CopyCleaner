// runtime_utils.hpp
#pragma once

#include "types_utils.hpp"
#include <optional>

namespace runtime_utils {

std::optional<double> to_f64(const RuntimeValue &v);

std::optional<RuntimeValue> numeric_add(const RuntimeValue &l, const RuntimeValue &r);
std::optional<RuntimeValue> numeric_sub(const RuntimeValue &l, const RuntimeValue &r);
std::optional<RuntimeValue> numeric_mul(const RuntimeValue &l, const RuntimeValue &r);
std::optional<RuntimeValue> numeric_div(const RuntimeValue &l, const RuntimeValue &r);
std::optional<RuntimeValue> numeric_pow(const RuntimeValue &l, const RuntimeValue &r);

std::optional<RuntimeValue> bool_and(const RuntimeValue &l, const RuntimeValue &r);
std::optional<RuntimeValue> bool_or(const RuntimeValue &l, const RuntimeValue &r);

std::optional<RuntimeValue> concat(const RuntimeValue &l, const RuntimeValue &r);

std::optional<RuntimeValue> compare_gt(const RuntimeValue &l, const RuntimeValue &r);
std::optional<RuntimeValue> compare_lt(const RuntimeValue &l, const RuntimeValue &r);
std::optional<RuntimeValue> compare_ge(const RuntimeValue &l, const RuntimeValue &r);
std::optional<RuntimeValue> compare_le(const RuntimeValue &l, const RuntimeValue &r);

}
