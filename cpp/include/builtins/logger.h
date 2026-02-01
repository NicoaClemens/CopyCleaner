// logger.h
// Declares: Logger built-in functions

#pragma once

#include <fstream>
#include <memory>
#include <optional>
#include <string>

#include "result.hpp"
#include "runtime_value.h"

namespace builtins {

/// @brief Manages logging functionality for the CopyCleaner interpreter
class Logger {
   private:
    std::optional<std::string> log_file_path;
    std::unique_ptr<std::ofstream> log_stream;

   public:
    Logger() = default;
    ~Logger();

    /// @brief Sets the log file path and opens the file for writing
    /// @param path The file path (relative or absolute) to write logs to
    /// @return Result containing true on success, or an error if the file cannot be opened
    Result<RuntimeValue> set_log(const std::string& path);

    /// @brief Logs a message to the configured log file with timestamp
    /// @param message The message to log
    /// @return Result containing null on success, or an error if no log file is set
    Result<RuntimeValue> log(const std::string& message);

    /// @brief Checks if a log file is currently configured
    /// @return true if a log file is set, false otherwise
    bool has_log_file() const;
};

}  // namespace builtins
