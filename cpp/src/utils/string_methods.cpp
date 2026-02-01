#include "../include/utils/string_methods.hpp"
#include "../include/errors.hpp"
#include <algorithm>
#include <cctype>

namespace StringMethods {

Result<RuntimeValue> length(const std::vector<RuntimeValue>& args) {
    if (args.size() != 1) {
        return err<RuntimeValue>(
            std::make_shared<Error>("length() expects 0 arguments", ErrorKind::Arity));
    }

    if (!std::holds_alternative<RuntimeValue::String>(args[0].value)) {
        return err<RuntimeValue>(std::make_shared<Error>(
            "length() can only be called on string type", ErrorKind::Type));
    }

    auto& str_val = std::get<RuntimeValue::String>(args[0].value);
    RuntimeValue result;
    result.value = RuntimeValue::Int{static_cast<int64_t>(str_val.value.length())};
    return ok(result);
}

Result<RuntimeValue> toUpper(const std::vector<RuntimeValue>& args) {
    if (args.size() != 1) {
        return err<RuntimeValue>(
            std::make_shared<Error>("toUpper() expects 0 arguments", ErrorKind::Arity));
    }

    if (!std::holds_alternative<RuntimeValue::String>(args[0].value)) {
        return err<RuntimeValue>(std::make_shared<Error>(
            "toUpper() can only be called on string type", ErrorKind::Type));
    }

    auto& str_val = std::get<RuntimeValue::String>(args[0].value);
    std::string upper = str_val.value;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);

    RuntimeValue result;
    result.value = RuntimeValue::String{upper};
    return ok(result);
}

Result<RuntimeValue> toLower(const std::vector<RuntimeValue>& args) {
    if (args.size() != 1) {
        return err<RuntimeValue>(
            std::make_shared<Error>("toLower() expects 0 arguments", ErrorKind::Arity));
    }

    if (!std::holds_alternative<RuntimeValue::String>(args[0].value)) {
        return err<RuntimeValue>(std::make_shared<Error>(
            "toLower() can only be called on string type", ErrorKind::Type));
    }

    auto& str_val = std::get<RuntimeValue::String>(args[0].value);
    std::string lower = str_val.value;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    RuntimeValue result;
    result.value = RuntimeValue::String{lower};
    return ok(result);
}

Result<RuntimeValue> trim(const std::vector<RuntimeValue>& args) {
    if (args.size() != 1) {
        return err<RuntimeValue>(
            std::make_shared<Error>("trim() expects 0 arguments", ErrorKind::Arity));
    }

    if (!std::holds_alternative<RuntimeValue::String>(args[0].value)) {
        return err<RuntimeValue>(std::make_shared<Error>(
            "trim() can only be called on string type", ErrorKind::Type));
    }

    auto& str_val = std::get<RuntimeValue::String>(args[0].value);
    std::string trimmed = str_val.value;

    // Trim leading whitespace
    trimmed.erase(trimmed.begin(),
                  std::find_if(trimmed.begin(), trimmed.end(),
                               [](unsigned char ch) { return !std::isspace(ch); }));

    // Trim trailing whitespace
    trimmed.erase(std::find_if(trimmed.rbegin(), trimmed.rend(),
                               [](unsigned char ch) { return !std::isspace(ch); })
                      .base(),
                  trimmed.end());

    RuntimeValue result;
    result.value = RuntimeValue::String{trimmed};
    return ok(result);
}

Result<RuntimeValue> substring(const std::vector<RuntimeValue>& args) {
    if (args.size() != 3) {
        return err<RuntimeValue>(
            std::make_shared<Error>("substring() expects 2 arguments", ErrorKind::Arity));
    }

    if (!std::holds_alternative<RuntimeValue::String>(args[0].value)) {
        return err<RuntimeValue>(std::make_shared<Error>(
            "substring() can only be called on string type", ErrorKind::Type));
    }

    if (!std::holds_alternative<RuntimeValue::Int>(args[1].value) ||
        !std::holds_alternative<RuntimeValue::Int>(args[2].value)) {
        return err<RuntimeValue>(std::make_shared<Error>(
            "substring() expects two integer arguments", ErrorKind::Type));
    }

    auto& str_val = std::get<RuntimeValue::String>(args[0].value);
    int64_t start = std::get<RuntimeValue::Int>(args[1].value).value;
    int64_t end = std::get<RuntimeValue::Int>(args[2].value).value;

    // Handle negative indices
    if (start < 0) start = static_cast<int64_t>(str_val.value.length()) + start;
    if (end < 0) end = static_cast<int64_t>(str_val.value.length()) + end;

    // Clamp to valid range
    start = std::max<int64_t>(0, std::min<int64_t>(start, str_val.value.length()));
    end = std::max<int64_t>(0, std::min<int64_t>(end, str_val.value.length()));

    if (start > end) {
        RuntimeValue result;
        result.value = RuntimeValue::String{""};
        return ok(result);
    }

    std::string substr =
        str_val.value.substr(static_cast<size_t>(start), static_cast<size_t>(end - start));
    RuntimeValue result;
    result.value = RuntimeValue::String{substr};
    return ok(result);
}

Result<RuntimeValue> replace(const std::vector<RuntimeValue>& args) {
    if (args.size() != 3) {
        return err<RuntimeValue>(
            std::make_shared<Error>("replace() expects 2 arguments", ErrorKind::Arity));
    }

    if (!std::holds_alternative<RuntimeValue::String>(args[0].value)) {
        return err<RuntimeValue>(std::make_shared<Error>(
            "replace() can only be called on string type", ErrorKind::Type));
    }

    if (!std::holds_alternative<RuntimeValue::String>(args[1].value) ||
        !std::holds_alternative<RuntimeValue::String>(args[2].value)) {
        return err<RuntimeValue>(std::make_shared<Error>(
            "replace() expects two string arguments", ErrorKind::Type));
    }

    auto& str_val = std::get<RuntimeValue::String>(args[0].value);
    auto& old_str = std::get<RuntimeValue::String>(args[1].value);
    auto& new_str = std::get<RuntimeValue::String>(args[2].value);

    std::string result_str = str_val.value;
    size_t pos = 0;
    while ((pos = result_str.find(old_str.value, pos)) != std::string::npos) {
        result_str.replace(pos, old_str.value.length(), new_str.value);
        pos += new_str.value.length();
    }

    RuntimeValue result;
    result.value = RuntimeValue::String{result_str};
    return ok(result);
}

Result<RuntimeValue> contains(const std::vector<RuntimeValue>& args) {
    if (args.size() != 2) {
        return err<RuntimeValue>(
            std::make_shared<Error>("contains() expects 1 argument", ErrorKind::Arity));
    }

    if (!std::holds_alternative<RuntimeValue::String>(args[0].value)) {
        return err<RuntimeValue>(std::make_shared<Error>(
            "contains() on string expects string type", ErrorKind::Type));
    }

    if (!std::holds_alternative<RuntimeValue::String>(args[1].value)) {
        return err<RuntimeValue>(std::make_shared<Error>(
            "contains() on string expects a string argument", ErrorKind::Type));
    }

    auto& str_val = std::get<RuntimeValue::String>(args[0].value);
    auto& search_str = std::get<RuntimeValue::String>(args[1].value);

    bool found = str_val.value.find(search_str.value) != std::string::npos;
    RuntimeValue result;
    result.value = RuntimeValue::Bool{found};
    return ok(result);
}

Result<RuntimeValue> startsWith(const std::vector<RuntimeValue>& args) {
    if (args.size() != 2) {
        return err<RuntimeValue>(
            std::make_shared<Error>("startsWith() expects 1 argument", ErrorKind::Arity));
    }

    if (!std::holds_alternative<RuntimeValue::String>(args[0].value)) {
        return err<RuntimeValue>(std::make_shared<Error>(
            "startsWith() can only be called on string type", ErrorKind::Type));
    }

    if (!std::holds_alternative<RuntimeValue::String>(args[1].value)) {
        return err<RuntimeValue>(std::make_shared<Error>(
            "startsWith() expects a string argument", ErrorKind::Type));
    }

    auto& str_val = std::get<RuntimeValue::String>(args[0].value);
    auto& prefix = std::get<RuntimeValue::String>(args[1].value);

    bool starts = str_val.value.size() >= prefix.value.size() &&
                  str_val.value.compare(0, prefix.value.size(), prefix.value) == 0;
    RuntimeValue result;
    result.value = RuntimeValue::Bool{starts};
    return ok(result);
}

Result<RuntimeValue> endsWith(const std::vector<RuntimeValue>& args) {
    if (args.size() != 2) {
        return err<RuntimeValue>(
            std::make_shared<Error>("endsWith() expects 1 argument", ErrorKind::Arity));
    }

    if (!std::holds_alternative<RuntimeValue::String>(args[0].value)) {
        return err<RuntimeValue>(std::make_shared<Error>(
            "endsWith() can only be called on string type", ErrorKind::Type));
    }

    if (!std::holds_alternative<RuntimeValue::String>(args[1].value)) {
        return err<RuntimeValue>(std::make_shared<Error>(
            "endsWith() expects a string argument", ErrorKind::Type));
    }

    auto& str_val = std::get<RuntimeValue::String>(args[0].value);
    auto& suffix = std::get<RuntimeValue::String>(args[1].value);

    bool ends = str_val.value.size() >= suffix.value.size() &&
                str_val.value.compare(str_val.value.size() - suffix.value.size(),
                                      suffix.value.size(), suffix.value) == 0;
    RuntimeValue result;
    result.value = RuntimeValue::Bool{ends};
    return ok(result);
}

Result<RuntimeValue> indexOf(const std::vector<RuntimeValue>& args) {
    if (args.size() != 2) {
        return err<RuntimeValue>(
            std::make_shared<Error>("indexOf() expects 1 argument", ErrorKind::Arity));
    }

    if (!std::holds_alternative<RuntimeValue::String>(args[0].value)) {
        return err<RuntimeValue>(std::make_shared<Error>(
            "indexOf() on string expects string type", ErrorKind::Type));
    }

    if (!std::holds_alternative<RuntimeValue::String>(args[1].value)) {
        return err<RuntimeValue>(std::make_shared<Error>(
            "indexOf() on string expects a string argument", ErrorKind::Type));
    }

    auto& str_val = std::get<RuntimeValue::String>(args[0].value);
    auto& search_str = std::get<RuntimeValue::String>(args[1].value);

    size_t pos = str_val.value.find(search_str.value);
    int64_t index = (pos == std::string::npos) ? -1 : static_cast<int64_t>(pos);

    RuntimeValue result;
    result.value = RuntimeValue::Int{index};
    return ok(result);
}

Result<RuntimeValue> split(const std::vector<RuntimeValue>& args) {
    if (args.size() != 2) {
        return err<RuntimeValue>(
            std::make_shared<Error>("split() expects 1 argument", ErrorKind::Arity));
    }

    if (!std::holds_alternative<RuntimeValue::String>(args[0].value)) {
        return err<RuntimeValue>(std::make_shared<Error>(
            "split() can only be called on string type", ErrorKind::Type));
    }

    if (!std::holds_alternative<RuntimeValue::String>(args[1].value)) {
        return err<RuntimeValue>(
            std::make_shared<Error>("split() expects a string delimiter", ErrorKind::Type));
    }

    auto& str_val = std::get<RuntimeValue::String>(args[0].value);
    auto& delimiter = std::get<RuntimeValue::String>(args[1].value);

    std::vector<RuntimeValue> parts;
    std::string text = str_val.value;
    size_t pos = 0;
    size_t found;

    if (delimiter.value.empty()) {
        // Split into individual characters
        for (char c : text) {
            RuntimeValue part;
            part.value = RuntimeValue::String{std::string(1, c)};
            parts.push_back(part);
        }
    } else {
        while ((found = text.find(delimiter.value, pos)) != std::string::npos) {
            RuntimeValue part;
            part.value = RuntimeValue::String{text.substr(pos, found - pos)};
            parts.push_back(part);
            pos = found + delimiter.value.length();
        }
        // Add remaining part
        RuntimeValue part;
        part.value = RuntimeValue::String{text.substr(pos)};
        parts.push_back(part);
    }

    RuntimeValue result;
    result.value = RuntimeValue::List{std::move(parts)};
    return ok(result);
}

Result<RuntimeValue> hasMatch(const std::vector<RuntimeValue>& args) {
    if (args.size() != 2) {
        return err<RuntimeValue>(
            std::make_shared<Error>("hasMatch() expects 1 argument", ErrorKind::Arity));
    }

    if (!std::holds_alternative<RuntimeValue::String>(args[0].value)) {
        return err<RuntimeValue>(std::make_shared<Error>(
            "hasMatch() can only be called on string type", ErrorKind::Type));
    }

    if (!std::holds_alternative<RuntimeValue::Match>(args[1].value)) {
        return err<RuntimeValue>(
            std::make_shared<Error>("hasMatch() expects a match argument", ErrorKind::Type));
    }

    auto& str_val = std::get<RuntimeValue::String>(args[0].value);
    auto& match_val = std::get<RuntimeValue::Match>(args[1].value);

    bool found = str_val.value.find(match_val.content) != std::string::npos;
    RuntimeValue result;
    result.value = RuntimeValue::Bool{found};
    return ok(result);
}

Result<RuntimeValue> replaceMatch(const std::vector<RuntimeValue>& args) {
    if (args.size() != 3) {
        return err<RuntimeValue>(std::make_shared<Error>(
            "replaceMatch() expects 2 arguments", ErrorKind::Arity));
    }

    if (!std::holds_alternative<RuntimeValue::String>(args[0].value)) {
        return err<RuntimeValue>(std::make_shared<Error>(
            "replaceMatch() can only be called on string type", ErrorKind::Type));
    }

    if (!std::holds_alternative<RuntimeValue::Match>(args[1].value)) {
        return err<RuntimeValue>(std::make_shared<Error>(
            "replaceMatch() expects a match as first argument", ErrorKind::Type));
    }

    if (!std::holds_alternative<RuntimeValue::String>(args[2].value)) {
        return err<RuntimeValue>(std::make_shared<Error>(
            "replaceMatch() expects a string as second argument", ErrorKind::Type));
    }

    auto& str_val = std::get<RuntimeValue::String>(args[0].value);
    auto& match_val = std::get<RuntimeValue::Match>(args[1].value);
    auto& replacement = std::get<RuntimeValue::String>(args[2].value);

    std::string result_str = str_val.value;
    if (match_val.start < result_str.length() && match_val.end <= result_str.length() &&
        match_val.start < match_val.end) {
        result_str.replace(match_val.start, match_val.end - match_val.start, replacement.value);
    }

    RuntimeValue result;
    result.value = RuntimeValue::String{result_str};
    return ok(result);
}

}  // namespace StringMethods
