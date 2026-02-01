// logger.cpp
// Implements builtins/logger.h

#include "builtins/logger.h"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "errors.hpp"

namespace builtins {

Logger::~Logger() {
    if (log_stream && log_stream->is_open()) {
        log_stream->close();
    }
}

Result<RuntimeValue> Logger::set_log(const std::string& path) {
    // Close existing stream if open
    if (log_stream && log_stream->is_open()) {
        log_stream->close();
    }

    // Open new log file in append mode
    log_stream = std::make_unique<std::ofstream>(path, std::ios::app);

    if (!log_stream->is_open()) {
        RuntimeValue result;
        result.value = RuntimeValue::Bool{false};
        return ok(result);
    }

    log_file_path = path;

    RuntimeValue result;
    result.value = RuntimeValue::Bool{true};
    return ok(result);
}

Result<RuntimeValue> Logger::log(const std::string& message) {
    if (!log_file_path.has_value() || !log_stream || !log_stream->is_open()) {
        return err<RuntimeValue>(std::make_shared<Error>(
            "No log file initialized. Call setLog() before logging.", ErrorKind::Runtime));
    }

    // Get current time
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    // Format: [year-month-day hour-minute-second-ms] : [message]
    std::ostringstream timestamp;
    std::tm tm_now;
#ifdef _WIN32
    localtime_s(&tm_now, &time_t_now);
#else
    localtime_r(&time_t_now, &tm_now);
#endif

    timestamp << "[" << std::put_time(&tm_now, "%Y-%m-%d %H:%M:%S") << ":" << std::setfill('0')
              << std::setw(3) << ms.count() << "] : [" << message << "]";

    // Write to log file
    *log_stream << timestamp.str() << std::endl;
    log_stream->flush();

    RuntimeValue result;
    result.value = RuntimeValue::Null{};
    return ok(result);
}

bool Logger::has_log_file() const {
    return log_file_path.has_value();
}

}  // namespace builtins
