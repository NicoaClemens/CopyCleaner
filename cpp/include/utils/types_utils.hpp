// types_utils.hpp
// Declares/Implements: operator==, to_string, is_truthy, matches_type

#pragma once

#include <cmath>
#include <cstdint>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "ast.h"
#include "runtime_value.h"

inline bool operator==(const RuntimeValue& a, const RuntimeValue& b) {
    // If types differ, allow implicit numeric comparison between Int and Float
    if (a.value.index() != b.value.index()) {
        // Int <-> Float implicit equality
        bool a_is_int = std::holds_alternative<RuntimeValue::Int>(a.value);
        bool a_is_float = std::holds_alternative<RuntimeValue::Float>(a.value);
        bool b_is_int = std::holds_alternative<RuntimeValue::Int>(b.value);
        bool b_is_float = std::holds_alternative<RuntimeValue::Float>(b.value);
        if ((a_is_int || a_is_float) && (b_is_int || b_is_float)) {
            double ad = a_is_int ? static_cast<double>(std::get<RuntimeValue::Int>(a.value).value)
                                  : std::get<RuntimeValue::Float>(a.value).value;
            double bd = b_is_int ? static_cast<double>(std::get<RuntimeValue::Int>(b.value).value)
                                  : std::get<RuntimeValue::Float>(b.value).value;
            return std::abs(ad - bd) < 1e-9;
        }
        return false;
    }

    switch (a.value.index()) {
        case 0: // Int
            return std::get<RuntimeValue::Int>(a.value).value == std::get<RuntimeValue::Int>(b.value).value;
        case 1: // Float
            return std::get<RuntimeValue::Float>(a.value).value == std::get<RuntimeValue::Float>(b.value).value;
        case 2: // Bool
            return std::get<RuntimeValue::Bool>(a.value).value == std::get<RuntimeValue::Bool>(b.value).value;
        case 3: // String
            return std::get<RuntimeValue::String>(a.value).value == std::get<RuntimeValue::String>(b.value).value;
        case 4: // List
            return std::get<RuntimeValue::List>(a.value).values == std::get<RuntimeValue::List>(b.value).values;
        case 5: { // Match
            const auto& l = std::get<RuntimeValue::Match>(a.value);
            const auto& r = std::get<RuntimeValue::Match>(b.value);
            return l.start == r.start && l.end == r.end && l.content == r.content;
        }
        case 6: { // Regex
            const auto& l = std::get<RuntimeValue::Regex>(a.value);
            const auto& r = std::get<RuntimeValue::Regex>(b.value);
            return l.re.flags == r.re.flags && l.re.literal == r.re.literal;
        }
        case 7: // Null
            return true;
        default:
            return false;
    }
}

inline std::string to_string(const RuntimeValue& v);

inline std::string to_string(const RuntimeValue& v) {
    return std::visit([](const auto& val) -> std::string {
        using T = std::decay_t<decltype(val)>;

        if constexpr (std::is_same_v<T, RuntimeValue::Int>)
            return std::to_string(val.value);

        else if constexpr (std::is_same_v<T, RuntimeValue::Float>)
            return std::to_string(val.value);

        else if constexpr (std::is_same_v<T, RuntimeValue::Bool>)
            return val.value ? "true" : "false";

        else if constexpr (std::is_same_v<T, RuntimeValue::String>)
            return val.value;

        else if constexpr (std::is_same_v<T, RuntimeValue::List>) {
            std::string out = "[";
            for (std::size_t i = 0; i < val.values.size(); ++i) {
                out += to_string(val.values[i]);
                if (i + 1 < val.values.size())
                    out += ", ";
            }
            out += "]";
            return out;
        }

        else if constexpr (std::is_same_v<T, RuntimeValue::Match>)
            return val.content;

        else if constexpr (std::is_same_v<T, RuntimeValue::Regex>)
            return "/" + val.re.literal + "/" + val.re.flags;

        else if constexpr (std::is_same_v<T, RuntimeValue::Null>)
            return "null";

        else
            return "";
    }, v.value);
}

inline bool is_truthy(const RuntimeValue& v) {
    return std::visit([](const auto& val) -> bool {
        using T = std::decay_t<decltype(val)>;

        if constexpr (std::is_same_v<T, RuntimeValue::Bool>)
            return val.value;

        else if constexpr (std::is_same_v<T, RuntimeValue::Int>)
            return val.value != 0;

        else if constexpr (std::is_same_v<T, RuntimeValue::Float>)
            return val.value != 0.0;

        else if constexpr (std::is_same_v<T, RuntimeValue::String>)
            return !val.value.empty();

        else if constexpr (std::is_same_v<T, RuntimeValue::List>)
            return !val.values.empty();

        else if constexpr (std::is_same_v<T, RuntimeValue::Match>)
            return true;

        else if constexpr (std::is_same_v<T, RuntimeValue::Regex>)
            return !val.re.literal.empty();

        else if constexpr (std::is_same_v<T, RuntimeValue::Null>)
            return false;

        else
            return false;
    }, v.value);
}

inline bool matches_type(const RuntimeValue& v, const AstType& t) {
    return std::visit([&](const auto& val) -> bool {
        using V = std::decay_t<decltype(val)>;

        return std::visit([&](const auto& ty) -> bool {
            using T = std::decay_t<decltype(ty)>;

            if constexpr (std::is_same_v<V, RuntimeValue::Int> && std::is_same_v<T, AstType::Int>)
                return true;

            else if constexpr (std::is_same_v<V, RuntimeValue::Float> && std::is_same_v<T, AstType::Float>)
                return true;

            else if constexpr (std::is_same_v<V, RuntimeValue::Bool> && std::is_same_v<T, AstType::Bool>)
                return true;

            else if constexpr (std::is_same_v<V, RuntimeValue::String> && std::is_same_v<T, AstType::String>)
                return true;

            else if constexpr (std::is_same_v<V, RuntimeValue::Regex> && std::is_same_v<T, AstType::Regex>)
                return true;

            else if constexpr (std::is_same_v<V, RuntimeValue::Match> && std::is_same_v<T, AstType::Match>)
                return true;

            else if constexpr (std::is_same_v<V, RuntimeValue::List> && std::is_same_v<T, AstType::List>) {
                // If the AST list type specifies an element type, ensure all elements match it
                const auto& rv = std::get<RuntimeValue::List>(v.value).values;
                if (!ty.element) return true;
                const AstType& elem_type = *ty.element;
                for (const auto& item : rv) {
                    if (!matches_type(item, elem_type)) return false;
                }
                return true;
            }

            else if constexpr (std::is_same_v<V, RuntimeValue::Null> && std::is_same_v<T, AstType::Null>)
                return true;

            // implicit numeric conversions
            else if constexpr (std::is_same_v<V, RuntimeValue::Int> && std::is_same_v<T, AstType::Float>)
                return true;

            else if constexpr (std::is_same_v<V, RuntimeValue::Float> && std::is_same_v<T, AstType::Int>)
                return true;

            else
                return false;

        }, t.value);

    }, v.value);
}