#pragma once

#include <iostream>
#include <string>
#include <sstream>

namespace utils {

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

class Logger {
public:
    static void log(LogLevel level, const std::string& message) {
        std::string prefix;
        switch (level) {
            case LogLevel::DEBUG:
                prefix = "[DEBUG] ";
                break;
            case LogLevel::INFO:
                prefix = "[INFO] ";
                break;
            case LogLevel::WARNING:
                prefix = "[WARNING] ";
                break;
            case LogLevel::ERROR:
                prefix = "[ERROR] ";
                break;
        }
        std::cout << prefix << message << std::endl;
    }

    static void debug(const std::string& message) {
        log(LogLevel::DEBUG, message);
    }

    static void info(const std::string& message) {
        log(LogLevel::INFO, message);
    }

    static void warning(const std::string& message) {
        log(LogLevel::WARNING, message);
    }

    static void error(const std::string& message) {
        log(LogLevel::ERROR, message);
    }
};

} // namespace utils

#define LOG_DEBUG(message) utils::Logger::debug(message)
#define LOG_INFO(message) utils::Logger::info(message)
#define LOG_WARNING(message) utils::Logger::warning(message)
#define LOG_WARN(message) utils::Logger::warning(message)
#define LOG_ERROR(message) utils::Logger::error(message) 