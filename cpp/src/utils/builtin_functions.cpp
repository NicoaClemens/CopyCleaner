// builtin_functions.cpp
// Implements builtin_functions.h

#include "utils/builtin_functions.h"

#include <sstream>

#include "builtins/alert.h"
#include "builtins/clipboard.h"
#include "builtins/console.h"
#include "builtins/logger.h"
#include "errors.hpp"
#include "runtime.h"
#include "utils/runtime_utils.h"

namespace builtin_functions {

Result<RuntimeValue> call_builtin(const std::string& name, const std::vector<RuntimeValue>& args,
                                  builtins::Logger& logger, builtins::Console& console,
                                  builtins::Clipboard& clipboard, builtins::Alert& alert,
                                  Interpreter* /* interp */) {
    if (name == "exit") {
        return ok(RuntimeValue{RuntimeValue::Null{}});
    }

    if (name == "fstring") {
        if (args.empty() || !std::holds_alternative<RuntimeValue::String>(args[0].value)) {
            return err<RuntimeValue>(std::make_shared<Error>(
                "first argument to fstring must be a string template", ErrorKind::Type));
        }
        std::string tpl = std::get<RuntimeValue::String>(args[0].value).value;
        std::string out;
        for (size_t i = 0; i < tpl.size(); ++i) {
            char c = tpl[i];
            if (c == '%' && i + 1 < tpl.size() &&
                std::isdigit(static_cast<unsigned char>(tpl[i + 1]))) {
                // parse number after '%'
                size_t j = i + 1;
                int num = 0;
                while (j < tpl.size() && std::isdigit(static_cast<unsigned char>(tpl[j]))) {
                    num = num * 10 + (tpl[j] - '0');
                    ++j;
                }
                // placeholder indices are 1-based and refer to following args
                // %1 refers to args[1] (first arg after template)
                if (num >= 1 && static_cast<size_t>(num) < args.size()) {
                    out += to_string(args[static_cast<size_t>(num)]);
                } else {
                    return err<RuntimeValue>(std::make_shared<Error>(
                        "fstring placeholder %" + std::to_string(num) + " out of range (only " +
                            std::to_string(args.size() - 1) + " arguments provided)",
                        ErrorKind::Runtime));
                }
                i = j - 1;
            } else {
                out.push_back(c);
            }
        }
        RuntimeValue result;
        result.value = RuntimeValue::String{out};
        return ok(result);
    }

    if (name == "setLog") {
        if (args.size() != 1) {
            return err<RuntimeValue>(
                std::make_shared<Error>("setLog() expects 1 argument", ErrorKind::Arity));
        }
        if (!std::holds_alternative<RuntimeValue::String>(args[0].value)) {
            return err<RuntimeValue>(
                std::make_shared<Error>("setLog() expects a string argument", ErrorKind::Type));
        }
        std::string path = std::get<RuntimeValue::String>(args[0].value).value;
        return logger.set_log(path);
    }

    if (name == "log") {
        if (args.size() != 1) {
            return err<RuntimeValue>(
                std::make_shared<Error>("log() expects 1 argument", ErrorKind::Arity));
        }
        std::string message = to_string(args[0]);
        return logger.log(message);
    }

    if (name == "print") {
        if (args.size() != 1) {
            return err<RuntimeValue>(
                std::make_shared<Error>("print() expects 1 argument", ErrorKind::Arity));
        }
        std::string message = to_string(args[0]);
        return console.print(message);
    }

    if (name == "clipboard_isText") {
        if (args.size() != 0) {
            return err<RuntimeValue>(std::make_shared<Error>(
                "clipboard_isText() expects no arguments", ErrorKind::Arity));
        }
        return clipboard.is_text();
    }

    if (name == "clipboard_read") {
        if (args.size() != 0) {
            return err<RuntimeValue>(
                std::make_shared<Error>("clipboard_read() expects no arguments", ErrorKind::Arity));
        }
        return clipboard.read();
    }

    if (name == "clipboard_write") {
        if (args.size() != 1) {
            return err<RuntimeValue>(
                std::make_shared<Error>("clipboard_write() expects 1 argument", ErrorKind::Arity));
        }
        if (!std::holds_alternative<RuntimeValue::String>(args[0].value)) {
            return err<RuntimeValue>(std::make_shared<Error>(
                "clipboard_write() expects a string argument", ErrorKind::Type));
        }
        std::string message = std::get<RuntimeValue::String>(args[0].value).value;
        return clipboard.write(message);
    }

    if (name == "showAlertOK") {
        if (args.size() != 2) {
            return err<RuntimeValue>(
                std::make_shared<Error>("showAlertOK() expects 2 arguments", ErrorKind::Arity));
        }
        std::string title = to_string(args[0]);
        std::string message = to_string(args[1]);
        return alert.show_ok(title, message);
    }

    if (name == "showAlert") {
        if (args.size() != 2) {
            return err<RuntimeValue>(
                std::make_shared<Error>("showAlert() expects 2 arguments", ErrorKind::Arity));
        }
        std::string title = to_string(args[0]);
        std::string message = to_string(args[1]);
        return alert.show_ok_cancel(title, message);
    }

    if (name == "showAlertYesNoCancel") {
        if (args.size() != 2) {
            return err<RuntimeValue>(std::make_shared<Error>(
                "showAlertYesNoCancel() expects 2 arguments", ErrorKind::Arity));
        }
        std::string title = to_string(args[0]);
        std::string message = to_string(args[1]);
        return alert.show_yes_no_cancel(title, message);
    }

    return err<RuntimeValue>(
        std::make_shared<Error>("unknown builtin function: " + name, ErrorKind::Runtime));
}

}  // namespace builtin_functions
