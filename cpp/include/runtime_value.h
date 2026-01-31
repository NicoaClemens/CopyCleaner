// runtime_value.h
// Declares: RegexType, RuntimeValue

#pragma once

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

struct RegexType {
    std::string literal;
    std::string flags;
};

struct RuntimeValue {
    struct Int {
        std::int64_t value;
    };
    struct Float {
        double value;
    };
    struct Bool {
        bool value;
    };
    struct String {
        std::string value;
    };
    struct List {
        std::vector<RuntimeValue> values;
    };
    struct Match {
        std::size_t start;
        std::size_t end;
        std::string content;
    };
    struct Regex {
        RegexType re;
    };
    struct Null {};

    using Variant = std::variant<Int, Float, Bool, String, List, Match, Regex, Null>;

    Variant value;
};
