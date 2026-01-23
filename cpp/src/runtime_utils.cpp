// runtime_utils.cpp

#include "runtime_utils.h"
#include <cmath>

namespace runtime_utils {

std::optional<double> to_f64(const RuntimeValue &v) {
    if (std::holds_alternative<RuntimeValue::Int>(v.value)) return (double)std::get<RuntimeValue::Int>(v.value).value;
    if (std::holds_alternative<RuntimeValue::Float>(v.value)) return std::get<RuntimeValue::Float>(v.value).value;
    return std::nullopt;
}

static RuntimeValue make_int(long long v) { RuntimeValue out; out.value = RuntimeValue::Int{ v }; return out; }
static RuntimeValue make_float(double v) { RuntimeValue out; out.value = RuntimeValue::Float{ v }; return out; }

std::optional<RuntimeValue> numeric_add(const RuntimeValue &l, const RuntimeValue &r) {
    if (std::holds_alternative<RuntimeValue::Int>(l.value) && std::holds_alternative<RuntimeValue::Int>(r.value)) {
        auto a = std::get<RuntimeValue::Int>(l.value).value;
        auto b = std::get<RuntimeValue::Int>(r.value).value;
        return make_int(a + b);
    }
    if (auto la = to_f64(l); la.has_value()) {
        if (auto rb = to_f64(r); rb.has_value()) return make_float(la.value() + rb.value());
    }
    return std::nullopt;
}

std::optional<RuntimeValue> numeric_sub(const RuntimeValue &l, const RuntimeValue &r) {
    if (std::holds_alternative<RuntimeValue::Int>(l.value) && std::holds_alternative<RuntimeValue::Int>(r.value)) {
        auto a = std::get<RuntimeValue::Int>(l.value).value;
        auto b = std::get<RuntimeValue::Int>(r.value).value;
        return make_int(a - b);
    }
    if (auto la = to_f64(l); la.has_value()) {
        if (auto rb = to_f64(r); rb.has_value()) return make_float(la.value() - rb.value());
    }
    return std::nullopt;
}

std::optional<RuntimeValue> numeric_mul(const RuntimeValue &l, const RuntimeValue &r) {
    if (std::holds_alternative<RuntimeValue::Int>(l.value) && std::holds_alternative<RuntimeValue::Int>(r.value)) {
        auto a = std::get<RuntimeValue::Int>(l.value).value;
        auto b = std::get<RuntimeValue::Int>(r.value).value;
        return make_int(a * b);
    }
    if (auto la = to_f64(l); la.has_value()) {
        if (auto rb = to_f64(r); rb.has_value()) return make_float(la.value() * rb.value());
    }
    return std::nullopt;
}

std::optional<RuntimeValue> numeric_div(const RuntimeValue &l, const RuntimeValue &r) {
    // check zero
    if (std::holds_alternative<RuntimeValue::Int>(r.value) && std::get<RuntimeValue::Int>(r.value).value == 0) return std::nullopt;
    if (std::holds_alternative<RuntimeValue::Float>(r.value) && std::get<RuntimeValue::Float>(r.value).value == 0.0) return std::nullopt;

    if (std::holds_alternative<RuntimeValue::Int>(l.value) && std::holds_alternative<RuntimeValue::Int>(r.value)) {
        auto a = std::get<RuntimeValue::Int>(l.value).value;
        auto b = std::get<RuntimeValue::Int>(r.value).value;
        return make_int(a / b);
    }
    if (auto la = to_f64(l); la.has_value()) {
        if (auto rb = to_f64(r); rb.has_value()) return make_float(la.value() / rb.value());
    }
    return std::nullopt;
}

std::optional<RuntimeValue> numeric_pow(const RuntimeValue &l, const RuntimeValue &r) {
    if (std::holds_alternative<RuntimeValue::Int>(l.value) && std::holds_alternative<RuntimeValue::Int>(r.value)) {
        auto a = std::get<RuntimeValue::Int>(l.value).value;
        auto b = std::get<RuntimeValue::Int>(r.value).value;
        if (b >= 0) {
            long long res = 1;
            for (long long i = 0; i < b; ++i) res *= a;
            return make_int(res);
        }
        return make_float(std::pow((double)a, (double)b));
    }
    if (auto la = to_f64(l); la.has_value()) {
        if (auto rb = to_f64(r); rb.has_value()) return make_float(std::pow(la.value(), rb.value()));
    }
    return std::nullopt;
}

std::optional<RuntimeValue> bool_and(const RuntimeValue &l, const RuntimeValue &r) {
    if (std::holds_alternative<RuntimeValue::Bool>(l.value) && std::holds_alternative<RuntimeValue::Bool>(r.value)) {
        bool a = std::get<RuntimeValue::Bool>(l.value).value;
        bool b = std::get<RuntimeValue::Bool>(r.value).value;
        RuntimeValue out; out.value = RuntimeValue::Bool{ a && b };
        return out;
    }
    return std::nullopt;
}

std::optional<RuntimeValue> bool_or(const RuntimeValue &l, const RuntimeValue &r) {
    if (std::holds_alternative<RuntimeValue::Bool>(l.value) && std::holds_alternative<RuntimeValue::Bool>(r.value)) {
        bool a = std::get<RuntimeValue::Bool>(l.value).value;
        bool b = std::get<RuntimeValue::Bool>(r.value).value;
        RuntimeValue out; out.value = RuntimeValue::Bool{ a || b };
        return out;
    }
    return std::nullopt;
}

std::optional<RuntimeValue> concat(const RuntimeValue &l, const RuntimeValue &r) {
    if (std::holds_alternative<RuntimeValue::String>(l.value) && std::holds_alternative<RuntimeValue::String>(r.value)) {
        auto a = std::get<RuntimeValue::String>(l.value).value;
        auto b = std::get<RuntimeValue::String>(r.value).value;
        RuntimeValue out; out.value = RuntimeValue::String{ a + b };
        return out;
    }
    RuntimeValue out; out.value = RuntimeValue::String{ to_string(l) + to_string(r) };
    return out;
}

std::optional<RuntimeValue> compare_gt(const RuntimeValue &l, const RuntimeValue &r) {
    if (auto a = to_f64(l); a.has_value()) {
        if (auto b = to_f64(r); b.has_value()) { RuntimeValue out; out.value = RuntimeValue::Bool{ a.value() > b.value() }; return out; }
    }
    if (std::holds_alternative<RuntimeValue::String>(l.value) && std::holds_alternative<RuntimeValue::String>(r.value)) {
        auto a = std::get<RuntimeValue::String>(l.value).value;
        auto b = std::get<RuntimeValue::String>(r.value).value;
        RuntimeValue out; out.value = RuntimeValue::Bool{ a > b }; return out;
    }
    return std::nullopt;
}

std::optional<RuntimeValue> compare_lt(const RuntimeValue &l, const RuntimeValue &r) {
    if (auto a = to_f64(l); a.has_value()) {
        if (auto b = to_f64(r); b.has_value()) { RuntimeValue out; out.value = RuntimeValue::Bool{ a.value() < b.value() }; return out; }
    }
    if (std::holds_alternative<RuntimeValue::String>(l.value) && std::holds_alternative<RuntimeValue::String>(r.value)) {
        auto a = std::get<RuntimeValue::String>(l.value).value;
        auto b = std::get<RuntimeValue::String>(r.value).value;
        RuntimeValue out; out.value = RuntimeValue::Bool{ a < b }; return out;
    }
    return std::nullopt;
}

std::optional<RuntimeValue> compare_ge(const RuntimeValue &l, const RuntimeValue &r) {
    if (auto a = to_f64(l); a.has_value()) {
        if (auto b = to_f64(r); b.has_value()) { RuntimeValue out; out.value = RuntimeValue::Bool{ a.value() >= b.value() }; return out; }
    }
    if (std::holds_alternative<RuntimeValue::String>(l.value) && std::holds_alternative<RuntimeValue::String>(r.value)) {
        auto a = std::get<RuntimeValue::String>(l.value).value;
        auto b = std::get<RuntimeValue::String>(r.value).value;
        RuntimeValue out; out.value = RuntimeValue::Bool{ a >= b }; return out;
    }
    return std::nullopt;
}

std::optional<RuntimeValue> compare_le(const RuntimeValue &l, const RuntimeValue &r) {
    if (auto a = to_f64(l); a.has_value()) {
        if (auto b = to_f64(r); b.has_value()) { RuntimeValue out; out.value = RuntimeValue::Bool{ a.value() <= b.value() }; return out; }
    }
    if (std::holds_alternative<RuntimeValue::String>(l.value) && std::holds_alternative<RuntimeValue::String>(r.value)) {
        auto a = std::get<RuntimeValue::String>(l.value).value;
        auto b = std::get<RuntimeValue::String>(r.value).value;
        RuntimeValue out; out.value = RuntimeValue::Bool{ a <= b }; return out;
    }
    return std::nullopt;
}

}
