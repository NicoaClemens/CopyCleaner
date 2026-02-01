// runtime_utils.cpp
// Implements runtime_utils.h

#include "utils/runtime_utils.h"

#include <cmath>

#include "utils/variant_utils.hpp"

namespace runtime_utils {

std::optional<double> to_f64(const RuntimeValue& v) {
    if (std::holds_alternative<RuntimeValue::Int>(v.value))
        return static_cast<double>(std::get<RuntimeValue::Int>(v.value).value);
    if (std::holds_alternative<RuntimeValue::Float>(v.value))
        return std::get<RuntimeValue::Float>(v.value).value;
    return std::nullopt;
}

static RuntimeValue make_int(long long v) {
    RuntimeValue out;
    out.value = RuntimeValue::Int{v};
    return out;
}
static RuntimeValue make_float(double v) {
    RuntimeValue out;
    out.value = RuntimeValue::Float{v};
    return out;
}

Result<RuntimeValue> numeric_add(const RuntimeValue& l, const RuntimeValue& r) {
    if (std::holds_alternative<RuntimeValue::Int>(l.value) &&
        std::holds_alternative<RuntimeValue::Int>(r.value)) {
        auto a = std::get<RuntimeValue::Int>(l.value).value;
        auto b = std::get<RuntimeValue::Int>(r.value).value;
        return ok(make_int(a + b));
    }
    if (auto la = to_f64(l); la.has_value()) {
        if (auto rb = to_f64(r); rb.has_value()) return ok(make_float(la.value() + rb.value()));
    }
    return err<RuntimeValue>(
        std::make_shared<Error>("unsupported operand types for +", ErrorKind::Type));
}

Result<RuntimeValue> numeric_sub(const RuntimeValue& l, const RuntimeValue& r) {
    if (std::holds_alternative<RuntimeValue::Int>(l.value) &&
        std::holds_alternative<RuntimeValue::Int>(r.value)) {
        auto a = std::get<RuntimeValue::Int>(l.value).value;
        auto b = std::get<RuntimeValue::Int>(r.value).value;
        return ok(make_int(a - b));
    }
    if (auto la = to_f64(l); la.has_value()) {
        if (auto rb = to_f64(r); rb.has_value()) return ok(make_float(la.value() - rb.value()));
    }
    return err<RuntimeValue>(
        std::make_shared<Error>("unsupported operand types for -", ErrorKind::Type));
}

Result<RuntimeValue> numeric_mul(const RuntimeValue& l, const RuntimeValue& r) {
    if (std::holds_alternative<RuntimeValue::Int>(l.value) &&
        std::holds_alternative<RuntimeValue::Int>(r.value)) {
        auto a = std::get<RuntimeValue::Int>(l.value).value;
        auto b = std::get<RuntimeValue::Int>(r.value).value;
        return ok(make_int(a * b));
    }
    if (auto la = to_f64(l); la.has_value()) {
        if (auto rb = to_f64(r); rb.has_value()) return ok(make_float(la.value() * rb.value()));
    }
    return err<RuntimeValue>(
        std::make_shared<Error>("unsupported operand types for *", ErrorKind::Type));
}

Result<RuntimeValue> numeric_div(const RuntimeValue& l, const RuntimeValue& r) {
    // check zero
    if (std::holds_alternative<RuntimeValue::Int>(r.value) &&
        std::get<RuntimeValue::Int>(r.value).value == 0)
        return err<RuntimeValue>(
            std::make_shared<Error>("division by zero", ErrorKind::DivideByZero));
    if (std::holds_alternative<RuntimeValue::Float>(r.value) &&
        std::get<RuntimeValue::Float>(r.value).value == 0.0)
        return err<RuntimeValue>(
            std::make_shared<Error>("division by zero", ErrorKind::DivideByZero));

    if (std::holds_alternative<RuntimeValue::Int>(l.value) &&
        std::holds_alternative<RuntimeValue::Int>(r.value)) {
        auto a = std::get<RuntimeValue::Int>(l.value).value;
        auto b = std::get<RuntimeValue::Int>(r.value).value;
        return ok(make_int(a / b));
    }
    if (auto la = to_f64(l); la.has_value()) {
        if (auto rb = to_f64(r); rb.has_value()) return ok(make_float(la.value() / rb.value()));
    }
    return err<RuntimeValue>(
        std::make_shared<Error>("unsupported operand types for /", ErrorKind::Type));
}

Result<RuntimeValue> numeric_pow(const RuntimeValue& l, const RuntimeValue& r) {
    // Use std::pow for all cases - cleaner and handles edge cases better
    if (auto la = to_f64(l); la.has_value()) {
        if (auto rb = to_f64(r); rb.has_value()) {
            double result = std::pow(la.value(), rb.value());
            // If both operands are ints and result is a whole number, return int
            if (std::holds_alternative<RuntimeValue::Int>(l.value) &&
                std::holds_alternative<RuntimeValue::Int>(r.value) &&
                std::get<RuntimeValue::Int>(r.value).value >= 0 &&
                result == std::floor(result)) {
                return ok(make_int(static_cast<long long>(result)));
            }
            return ok(make_float(result));
        }
    }
    return err<RuntimeValue>(
        std::make_shared<Error>("unsupported operand types for **", ErrorKind::Type));
}

Result<RuntimeValue> concat(const RuntimeValue& l, const RuntimeValue& r) {
    if (std::holds_alternative<RuntimeValue::String>(l.value) &&
        std::holds_alternative<RuntimeValue::String>(r.value)) {
        auto a = std::get<RuntimeValue::String>(l.value).value;
        auto b = std::get<RuntimeValue::String>(r.value).value;
        RuntimeValue out;
        out.value = RuntimeValue::String{a + b};
        return ok(out);
    }
    RuntimeValue out;
    out.value = RuntimeValue::String{to_string(l) + to_string(r)};
    return ok(out);
}

Result<RuntimeValue> compare_gt(const RuntimeValue& l, const RuntimeValue& r) {
    if (auto a = to_f64(l); a.has_value()) {
        if (auto b = to_f64(r); b.has_value()) {
            RuntimeValue out;
            out.value = RuntimeValue::Bool{a.value() > b.value()};
            return ok(out);
        }
    }
    if (std::holds_alternative<RuntimeValue::String>(l.value) &&
        std::holds_alternative<RuntimeValue::String>(r.value)) {
        auto a = std::get<RuntimeValue::String>(l.value).value;
        auto b = std::get<RuntimeValue::String>(r.value).value;
        RuntimeValue out;
        out.value = RuntimeValue::Bool{a > b};
        return ok(out);
    }
    return err<RuntimeValue>(
        std::make_shared<Error>("unsupported operand types for >", ErrorKind::Type));
}

Result<RuntimeValue> compare_lt(const RuntimeValue& l, const RuntimeValue& r) {
    if (auto a = to_f64(l); a.has_value()) {
        if (auto b = to_f64(r); b.has_value()) {
            RuntimeValue out;
            out.value = RuntimeValue::Bool{a.value() < b.value()};
            return ok(out);
        }
    }
    if (std::holds_alternative<RuntimeValue::String>(l.value) &&
        std::holds_alternative<RuntimeValue::String>(r.value)) {
        auto a = std::get<RuntimeValue::String>(l.value).value;
        auto b = std::get<RuntimeValue::String>(r.value).value;
        RuntimeValue out;
        out.value = RuntimeValue::Bool{a < b};
        return ok(out);
    }
    return err<RuntimeValue>(
        std::make_shared<Error>("unsupported operand types for <", ErrorKind::Type));
}

Result<RuntimeValue> compare_ge(const RuntimeValue& l, const RuntimeValue& r) {
    if (auto a = to_f64(l); a.has_value()) {
        if (auto b = to_f64(r); b.has_value()) {
            RuntimeValue out;
            out.value = RuntimeValue::Bool{a.value() >= b.value()};
            return ok(out);
        }
    }
    if (std::holds_alternative<RuntimeValue::String>(l.value) &&
        std::holds_alternative<RuntimeValue::String>(r.value)) {
        auto a = std::get<RuntimeValue::String>(l.value).value;
        auto b = std::get<RuntimeValue::String>(r.value).value;
        RuntimeValue out;
        out.value = RuntimeValue::Bool{a >= b};
        return ok(out);
    }
    return err<RuntimeValue>(
        std::make_shared<Error>("unsupported operand types for >=", ErrorKind::Type));
}

Result<RuntimeValue> compare_le(const RuntimeValue& l, const RuntimeValue& r) {
    if (auto a = to_f64(l); a.has_value()) {
        if (auto b = to_f64(r); b.has_value()) {
            RuntimeValue out;
            out.value = RuntimeValue::Bool{a.value() <= b.value()};
            return ok(out);
        }
    }
    if (std::holds_alternative<RuntimeValue::String>(l.value) &&
        std::holds_alternative<RuntimeValue::String>(r.value)) {
        auto a = std::get<RuntimeValue::String>(l.value).value;
        auto b = std::get<RuntimeValue::String>(r.value).value;
        RuntimeValue out;
        out.value = RuntimeValue::Bool{a <= b};
        return ok(out);
    }
    return err<RuntimeValue>(
        std::make_shared<Error>("unsupported operand types for <=", ErrorKind::Type));
}

Result<RuntimeValue> eval_binary_op(Operator op, const RuntimeValue& left, const RuntimeValue& right) {
    switch (op) {
        case Operator::Add:
            return numeric_add(left, right);
        case Operator::Sub:
            return numeric_sub(left, right);
        case Operator::Mul:
            return numeric_mul(left, right);
        case Operator::Div:
            return numeric_div(left, right);
        case Operator::Pow:
            return numeric_pow(left, right);
        case Operator::Eq: {
            RuntimeValue out;
            out.value = RuntimeValue::Bool{left == right};
            return ok(out);
        }
        case Operator::Ne: {
            RuntimeValue out;
            out.value = RuntimeValue::Bool{left != right};
            return ok(out);
        }
        case Operator::Gt:
            return compare_gt(left, right);
        case Operator::Lt:
            return compare_lt(left, right);
        case Operator::Ge:
            return compare_ge(left, right);
        case Operator::Le:
            return compare_le(left, right);
        case Operator::Concat:
            return concat(left, right);
        default:
            return err<RuntimeValue>(
                std::make_shared<Error>("unsupported binary operator", ErrorKind::Runtime));
    }
}

Result<RuntimeValue> cast_value(const RuntimeValue& val, const AstType& target_type) {
    return std::visit(
        overloaded{
            [&val](const AstType::Int&) -> Result<RuntimeValue> {
                if (std::holds_alternative<RuntimeValue::Int>(val.value)) {
                    return ok(val);
                }
                if (std::holds_alternative<RuntimeValue::Float>(val.value)) {
                    auto f = std::get<RuntimeValue::Float>(val.value).value;
                    RuntimeValue result;
                    result.value = RuntimeValue::Int{static_cast<int64_t>(f)};
                    return ok(result);
                }
                if (std::holds_alternative<RuntimeValue::Bool>(val.value)) {
                    auto b = std::get<RuntimeValue::Bool>(val.value).value;
                    RuntimeValue result;
                    result.value = RuntimeValue::Int{b ? 1 : 0};
                    return ok(result);
                }
                return err<RuntimeValue>(std::make_shared<Error>(
                    "cannot cast to int from this type", ErrorKind::Type));
            },
            [&val](const AstType::Float&) -> Result<RuntimeValue> {
                if (std::holds_alternative<RuntimeValue::Float>(val.value)) {
                    return ok(val);
                }
                if (std::holds_alternative<RuntimeValue::Int>(val.value)) {
                    auto i = std::get<RuntimeValue::Int>(val.value).value;
                    RuntimeValue result;
                    result.value = RuntimeValue::Float{static_cast<double>(i)};
                    return ok(result);
                }
                return err<RuntimeValue>(std::make_shared<Error>(
                    "cannot cast to float from this type", ErrorKind::Type));
            },
            [&val](const AstType::String&) -> Result<RuntimeValue> {
                RuntimeValue result;
                result.value = RuntimeValue::String{to_string(val)};
                return ok(result);
            },
            [&val](const AstType::Bool&) -> Result<RuntimeValue> {
                RuntimeValue result;
                result.value = RuntimeValue::Bool{is_truthy(val)};
                return ok(result);
            },
            [](const auto&) -> Result<RuntimeValue> {
                return err<RuntimeValue>(std::make_shared<Error>(
                    "type casting not supported for this target type", ErrorKind::Type));
            }
        },
        target_type.value);
}

Result<RuntimeValue> access_member(const RuntimeValue& obj, const std::string& member) {
    // Regex members
    if (std::holds_alternative<RuntimeValue::Regex>(obj.value)) {
        auto& regex_val = std::get<RuntimeValue::Regex>(obj.value);
        if (member == "re") {
            RuntimeValue result;
            result.value = RuntimeValue::String{regex_val.re.literal};
            return ok(result);
        }
        if (member == "flags") {
            RuntimeValue result;
            result.value = RuntimeValue::String{regex_val.re.flags};
            return ok(result);
        }
        return err<RuntimeValue>(std::make_shared<Error>(
            "regex type has no member '" + member + "'", ErrorKind::Runtime));
    }

    // Match members
    if (std::holds_alternative<RuntimeValue::Match>(obj.value)) {
        auto& match_val = std::get<RuntimeValue::Match>(obj.value);
        if (member == "start") {
            RuntimeValue result;
            result.value = RuntimeValue::Int{static_cast<int64_t>(match_val.start)};
            return ok(result);
        }
        if (member == "end") {
            RuntimeValue result;
            result.value = RuntimeValue::Int{static_cast<int64_t>(match_val.end)};
            return ok(result);
        }
        if (member == "content") {
            RuntimeValue result;
            result.value = RuntimeValue::String{match_val.content};
            return ok(result);
        }
        return err<RuntimeValue>(std::make_shared<Error>(
            "match type has no member '" + member + "'", ErrorKind::Runtime));
    }

    // List error
    if (std::holds_alternative<RuntimeValue::List>(obj.value)) {
        return err<RuntimeValue>(std::make_shared<Error>(
            "list member access requires method call syntax (e.g., .get(index))",
            ErrorKind::Runtime));
    }

    return err<RuntimeValue>(std::make_shared<Error>(
        "member access not supported for this type", ErrorKind::Runtime));
}

}  // namespace runtime_utils
