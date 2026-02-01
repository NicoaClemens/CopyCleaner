#include "../include/utils/list_methods.hpp"

#include <algorithm>

#include "../include/errors.hpp"

namespace ListMethods {

Result<RuntimeValue> length(const std::vector<RuntimeValue>& args) {
    if (args.size() != 1) {
        return err<RuntimeValue>(
            std::make_shared<Error>("length() expects 0 arguments", ErrorKind::Arity));
    }

    if (!std::holds_alternative<RuntimeValue::List>(args[0].value)) {
        return err<RuntimeValue>(
            std::make_shared<Error>("length() can only be called on list type", ErrorKind::Type));
    }

    auto& list_val = std::get<RuntimeValue::List>(args[0].value);
    RuntimeValue result;
    result.value = RuntimeValue::Int{static_cast<int64_t>(list_val.values.size())};
    return ok(result);
}

Result<RuntimeValue> get(const std::vector<RuntimeValue>& args) {
    if (args.size() != 2) {
        return err<RuntimeValue>(
            std::make_shared<Error>("get() expects 1 argument", ErrorKind::Arity));
    }

    if (!std::holds_alternative<RuntimeValue::List>(args[0].value)) {
        return err<RuntimeValue>(
            std::make_shared<Error>("get() can only be called on list type", ErrorKind::Type));
    }

    if (!std::holds_alternative<RuntimeValue::Int>(args[1].value)) {
        return err<RuntimeValue>(
            std::make_shared<Error>("get() expects an integer index", ErrorKind::Type));
    }

    auto& list_val = std::get<RuntimeValue::List>(args[0].value);
    int64_t index = std::get<RuntimeValue::Int>(args[1].value).value;

    // Handle negative indices (count from end)
    if (index < 0) {
        index = static_cast<int64_t>(list_val.values.size()) + index;
    }

    if (index < 0 || index >= static_cast<int64_t>(list_val.values.size())) {
        return err<RuntimeValue>(
            std::make_shared<Error>("list index out of range", ErrorKind::Runtime));
    }

    return ok(list_val.values[static_cast<size_t>(index)]);
}

Result<RuntimeValue> push(const std::vector<RuntimeValue>& args) {
    if (args.size() != 2) {
        return err<RuntimeValue>(
            std::make_shared<Error>("push() expects 1 argument", ErrorKind::Arity));
    }

    if (!std::holds_alternative<RuntimeValue::List>(args[0].value)) {
        return err<RuntimeValue>(
            std::make_shared<Error>("push() can only be called on list type", ErrorKind::Type));
    }

    auto list_val = std::get<RuntimeValue::List>(args[0].value);
    list_val.values.push_back(args[1]);

    RuntimeValue result;
    result.value = list_val;
    return ok(result);
}

Result<RuntimeValue> slice(const std::vector<RuntimeValue>& args) {
    if (args.size() != 3) {
        return err<RuntimeValue>(
            std::make_shared<Error>("slice() expects 2 arguments", ErrorKind::Arity));
    }

    if (!std::holds_alternative<RuntimeValue::List>(args[0].value)) {
        return err<RuntimeValue>(
            std::make_shared<Error>("slice() can only be called on list type", ErrorKind::Type));
    }

    if (!std::holds_alternative<RuntimeValue::Int>(args[1].value) ||
        !std::holds_alternative<RuntimeValue::Int>(args[2].value)) {
        return err<RuntimeValue>(
            std::make_shared<Error>("slice() expects two integer arguments", ErrorKind::Type));
    }

    auto& list_val = std::get<RuntimeValue::List>(args[0].value);
    int64_t start = std::get<RuntimeValue::Int>(args[1].value).value;
    int64_t end = std::get<RuntimeValue::Int>(args[2].value).value;

    // Handle negative indices
    if (start < 0) start = static_cast<int64_t>(list_val.values.size()) + start;
    if (end < 0) end = static_cast<int64_t>(list_val.values.size()) + end;

    // Clamp to valid range
    start = std::max<int64_t>(0, std::min<int64_t>(start, list_val.values.size()));
    end = std::max<int64_t>(0, std::min<int64_t>(end, list_val.values.size()));

    std::vector<RuntimeValue> sliced;
    if (start < end) {
        sliced.insert(sliced.end(), list_val.values.begin() + start, list_val.values.begin() + end);
    }

    RuntimeValue result;
    result.value = RuntimeValue::List{std::move(sliced)};
    return ok(result);
}

// Helper function to compare RuntimeValues for equality
static bool valuesEqual(const RuntimeValue& a, const RuntimeValue& b) {
    if (a.value.index() != b.value.index()) {
        return false;
    }

    if (std::holds_alternative<RuntimeValue::Int>(a.value)) {
        return std::get<RuntimeValue::Int>(a.value).value ==
               std::get<RuntimeValue::Int>(b.value).value;
    } else if (std::holds_alternative<RuntimeValue::Float>(a.value)) {
        return std::get<RuntimeValue::Float>(a.value).value ==
               std::get<RuntimeValue::Float>(b.value).value;
    } else if (std::holds_alternative<RuntimeValue::String>(a.value)) {
        return std::get<RuntimeValue::String>(a.value).value ==
               std::get<RuntimeValue::String>(b.value).value;
    } else if (std::holds_alternative<RuntimeValue::Bool>(a.value)) {
        return std::get<RuntimeValue::Bool>(a.value).value ==
               std::get<RuntimeValue::Bool>(b.value).value;
    }

    return false;
}

Result<RuntimeValue> contains(const std::vector<RuntimeValue>& args) {
    if (args.size() != 2) {
        return err<RuntimeValue>(
            std::make_shared<Error>("contains() expects 1 argument", ErrorKind::Arity));
    }

    if (!std::holds_alternative<RuntimeValue::List>(args[0].value)) {
        return err<RuntimeValue>(
            std::make_shared<Error>("contains() on list expects list type", ErrorKind::Type));
    }

    auto& list_val = std::get<RuntimeValue::List>(args[0].value);
    bool found = false;

    for (const auto& elem : list_val.values) {
        if (valuesEqual(elem, args[1])) {
            found = true;
            break;
        }
    }

    RuntimeValue result;
    result.value = RuntimeValue::Bool{found};
    return ok(result);
}

Result<RuntimeValue> indexOf(const std::vector<RuntimeValue>& args) {
    if (args.size() != 2) {
        return err<RuntimeValue>(
            std::make_shared<Error>("indexOf() expects 1 argument", ErrorKind::Arity));
    }

    if (!std::holds_alternative<RuntimeValue::List>(args[0].value)) {
        return err<RuntimeValue>(
            std::make_shared<Error>("indexOf() on list expects list type", ErrorKind::Type));
    }

    auto& list_val = std::get<RuntimeValue::List>(args[0].value);
    int64_t index = -1;

    for (size_t i = 0; i < list_val.values.size(); ++i) {
        if (valuesEqual(list_val.values[i], args[1])) {
            index = static_cast<int64_t>(i);
            break;
        }
    }

    RuntimeValue result;
    result.value = RuntimeValue::Int{index};
    return ok(result);
}

}  // namespace ListMethods
