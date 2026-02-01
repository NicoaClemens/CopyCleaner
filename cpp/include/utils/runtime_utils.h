// runtime_utils.h
// Declares: to_f64, numeric_add, numeric_sub, numeric_mul, numeric_div, numeric_pow, concat,
// compare_gt, compare_lt, compare_ge, compare_le, eval_binary_op, cast_value, access_member

#pragma once

#include <optional>

#include "result.hpp"
#include "types_utils.hpp"

/// @brief utility functions for runtime.cpp
namespace runtime_utils {

std::optional<double> to_f64(const RuntimeValue& v);

Result<RuntimeValue> numeric_add(const RuntimeValue& l, const RuntimeValue& r);
Result<RuntimeValue> numeric_sub(const RuntimeValue& l, const RuntimeValue& r);
Result<RuntimeValue> numeric_mul(const RuntimeValue& l, const RuntimeValue& r);
Result<RuntimeValue> numeric_div(const RuntimeValue& l, const RuntimeValue& r);
Result<RuntimeValue> numeric_pow(const RuntimeValue& l, const RuntimeValue& r);

Result<RuntimeValue> concat(const RuntimeValue& l, const RuntimeValue& r);

Result<RuntimeValue> compare_gt(const RuntimeValue& l, const RuntimeValue& r);
Result<RuntimeValue> compare_lt(const RuntimeValue& l, const RuntimeValue& r);
Result<RuntimeValue> compare_ge(const RuntimeValue& l, const RuntimeValue& r);
Result<RuntimeValue> compare_le(const RuntimeValue& l, const RuntimeValue& r);

/// @brief Evaluates a binary operation on two runtime values
/// @param op The operator to apply
/// @param left The left operand
/// @param right The right operand
/// @return Result containing the computed value or an error
Result<RuntimeValue> eval_binary_op(Operator op, const RuntimeValue& left, const RuntimeValue& right);

/// @brief Casts a runtime value to a different type
/// @param value The value to cast
/// @param target_type The target type to cast to
/// @return Result containing the casted value or an error
Result<RuntimeValue> cast_value(const RuntimeValue& value, const AstType& target_type);

/// @brief Accesses a member property of a runtime value (regex.re, match.start, etc.)
/// @param object The object to access the member from
/// @param member_name The name of the member to access
/// @return Result containing the member value or an error
Result<RuntimeValue> access_member(const RuntimeValue& object, const std::string& member_name);

}  // namespace runtime_utils
