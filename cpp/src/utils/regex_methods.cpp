#include "../include/utils/regex_methods.hpp"

#include <regex>

#include "../include/errors.hpp"

namespace RegexMethods {

Result<RuntimeValue> getAll(const std::vector<RuntimeValue>& args) {
    if (args.size() != 2) {
        return err<RuntimeValue>(
            std::make_shared<Error>("getAll() expects 1 argument", ErrorKind::Arity));
    }

    if (!std::holds_alternative<RuntimeValue::Regex>(args[0].value)) {
        return err<RuntimeValue>(
            std::make_shared<Error>("getAll() can only be called on regex type", ErrorKind::Type));
    }

    if (!std::holds_alternative<RuntimeValue::String>(args[1].value)) {
        return err<RuntimeValue>(
            std::make_shared<Error>("getAll() expects a string argument", ErrorKind::Type));
    }

    auto& regex_val = std::get<RuntimeValue::Regex>(args[0].value);
    std::string text = std::get<RuntimeValue::String>(args[1].value).value;

    // Convert regex pattern and flags to std::regex
    std::regex::flag_type flags = std::regex::ECMAScript;
    for (char c : regex_val.re.flags) {
        if (c == 'i') flags |= std::regex::icase;
        // Add more flag support as needed
    }

    try {
        std::regex re(regex_val.re.literal, flags);
        std::vector<RuntimeValue> matches;

        auto words_begin = std::sregex_iterator(text.begin(), text.end(), re);
        auto words_end = std::sregex_iterator();

        for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
            std::smatch match = *i;
            RuntimeValue match_val;
            match_val.value = RuntimeValue::Match{
                static_cast<size_t>(match.position()),
                static_cast<size_t>(match.position() + match.length()), match.str()};
            matches.push_back(match_val);
        }

        RuntimeValue result;
        result.value = RuntimeValue::List{std::move(matches)};
        return ok(result);
    } catch (const std::regex_error& e) {
        return err<RuntimeValue>(
            std::make_shared<Error>(std::string("regex error: ") + e.what(), ErrorKind::Runtime));
    }
}

}  // namespace RegexMethods
