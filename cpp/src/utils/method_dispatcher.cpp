#include "../include/utils/method_dispatcher.hpp"

#include "../include/errors.hpp"
#include "../include/utils/list_methods.hpp"
#include "../include/utils/regex_methods.hpp"
#include "../include/utils/string_methods.hpp"

namespace MethodDispatcher {

Result<RuntimeValue> dispatchMethod(const std::string& methodName,
                                    const std::vector<RuntimeValue>& args) {
    // Handle methods that work on multiple types
    if (methodName == "__method_length") {
        if (args.size() < 1) {
            return err<RuntimeValue>(
                std::make_shared<Error>("length() expects 0 arguments", ErrorKind::Arity));
        }

        if (std::holds_alternative<RuntimeValue::String>(args[0].value)) {
            return StringMethods::length(args);
        } else if (std::holds_alternative<RuntimeValue::List>(args[0].value)) {
            return ListMethods::length(args);
        } else {
            return err<RuntimeValue>(std::make_shared<Error>(
                "length() can only be called on string or list type", ErrorKind::Type));
        }
    }

    if (methodName == "__method_contains") {
        if (args.size() < 1) {
            return err<RuntimeValue>(
                std::make_shared<Error>("contains() expects 1 argument", ErrorKind::Arity));
        }

        if (std::holds_alternative<RuntimeValue::String>(args[0].value)) {
            return StringMethods::contains(args);
        } else if (std::holds_alternative<RuntimeValue::List>(args[0].value)) {
            return ListMethods::contains(args);
        } else {
            return err<RuntimeValue>(std::make_shared<Error>(
                "contains() can only be called on string or list type", ErrorKind::Type));
        }
    }

    if (methodName == "__method_indexOf") {
        if (args.size() < 1) {
            return err<RuntimeValue>(
                std::make_shared<Error>("indexOf() expects 1 argument", ErrorKind::Arity));
        }

        if (std::holds_alternative<RuntimeValue::String>(args[0].value)) {
            return StringMethods::indexOf(args);
        } else if (std::holds_alternative<RuntimeValue::List>(args[0].value)) {
            return ListMethods::indexOf(args);
        } else {
            return err<RuntimeValue>(std::make_shared<Error>(
                "indexOf() can only be called on string or list type", ErrorKind::Type));
        }
    }

    // String methods
    if (methodName == "__method_toUpper") {
        return StringMethods::toUpper(args);
    }
    if (methodName == "__method_toLower") {
        return StringMethods::toLower(args);
    }
    if (methodName == "__method_trim") {
        return StringMethods::trim(args);
    }
    if (methodName == "__method_substring") {
        return StringMethods::substring(args);
    }
    if (methodName == "__method_replace") {
        return StringMethods::replace(args);
    }
    if (methodName == "__method_startsWith") {
        return StringMethods::startsWith(args);
    }
    if (methodName == "__method_endsWith") {
        return StringMethods::endsWith(args);
    }
    if (methodName == "__method_split") {
        return StringMethods::split(args);
    }
    if (methodName == "__method_hasMatch") {
        return StringMethods::hasMatch(args);
    }
    if (methodName == "__method_replaceMatch") {
        return StringMethods::replaceMatch(args);
    }

    // List methods
    if (methodName == "__method_get") {
        return ListMethods::get(args);
    }
    if (methodName == "__method_push") {
        return ListMethods::push(args);
    }
    if (methodName == "__method_slice") {
        return ListMethods::slice(args);
    }

    // Regex methods
    if (methodName == "__method_getAll") {
        return RegexMethods::getAll(args);
    }

    // Method not found
    return err<RuntimeValue>(
        std::make_shared<Error>("Unknown method: " + methodName, ErrorKind::Runtime));
}

}  // namespace MethodDispatcher
