// runtime_value.hpp

#include <cstdint>
#include <regex>
#include <string>
#include <variant>
#include <vector>

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
        std::regex re;
        std::string flags;
    };

    struct Null {};

    using Variant = std::variant<
        Int, Float, Bool, String, List, Match, Regex, Null>;

    Variant value;
};
