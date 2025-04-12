#pragma once
#include <string>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace SFE {

class Logger {
public:
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    void initialize(const std::string& logFilePath) {
        if (logFile.is_open()) {
            logFile.close();
        }
        logFile.open(logFilePath, std::ios::out | std::ios::app);
        logStartTime = std::chrono::high_resolution_clock::now();
        
        // Write CSV header
        logFile << "Timestamp,FPS,SceneRenderTime_ms,TextRenderTime_ms,TextDrawCalls,TotalCharacters,StressTestEnabled\n";
    }

    void logPerformanceMetrics(int fps, float sceneRenderTime, float textRenderTime, 
                             int textDrawCalls, int totalCharacters, bool stressTestEnabled) {
        if (!logFile.is_open()) return;

        auto now = std::chrono::high_resolution_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now - logStartTime).count();

        logFile << timestamp << ","
                << fps << ","
                << std::fixed << std::setprecision(3)
                << sceneRenderTime * 1000.0f << ","
                << textRenderTime * 1000.0f << ","
                << textDrawCalls << ","
                << totalCharacters << ","
                << (stressTestEnabled ? "1" : "0") << "\n";
        
        logFile.flush();
    }

    void logMessage(const std::string& message) {
        if (!logFile.is_open()) return;

        auto now = std::chrono::high_resolution_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now - logStartTime).count();

        logFile << timestamp << ",INFO,\"" << message << "\"\n";
        logFile.flush();
    }

    ~Logger() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }

private:
    Logger() = default;
    std::ofstream logFile;
    std::chrono::high_resolution_clock::time_point logStartTime;

    // Delete copy constructor and assignment operator
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
};

} // namespace SFE 