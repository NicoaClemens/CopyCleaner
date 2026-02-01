// include/builtins/alert.h
// Declares: Alert Class

#pragma once

#include <string>

#include "result.hpp"
#include "runtime_value.h"

namespace builtins {

class Alert {
   public:
    Result<RuntimeValue> show_ok(const std::string& title, const std::string& message);
    Result<RuntimeValue> show_ok_cancel(const std::string& title, const std::string& message);
    Result<RuntimeValue> show_yes_no_cancel(const std::string& title, const std::string& message);

   private:
    // Platform-agnostic internal method
    int show_dialog(const std::string& title, const std::string& message, int button_type);
};

}  // namespace builtins