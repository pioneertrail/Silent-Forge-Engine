#include "utils/log.h"
#include <iostream>
#include <ctime>

namespace utils {

// Static implementation of Logger methods

void Logger::log(LogLevel level, const std::string& message) {
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
    
    // Add timestamp
    time_t now = time(nullptr);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
    
    std::cout << "[" << timestamp << "] " << prefix << message << std::endl;
}

void Logger::debug(const std::string& message) {
    log(LogLevel::DEBUG, message);
}

void Logger::info(const std::string& message) {
    log(LogLevel::INFO, message);
}

void Logger::warning(const std::string& message) {
    log(LogLevel::WARNING, message);
}

void Logger::error(const std::string& message) {
    log(LogLevel::ERROR, message);
}

} // namespace utils 