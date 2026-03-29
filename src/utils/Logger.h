#pragma once
#include <string>
#include <fstream>
#include <mutex>
#include <ctime>

enum class LogLevel { INFO, WARNING, ERROR_L };

class Logger
{
public:
    static Logger& getInstance();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void log(LogLevel level, const std::string& message);

    void info(const std::string& msg) { log(LogLevel::INFO, msg); }
    void warning(const std::string& msg) { log(LogLevel::WARNING, msg); }
    void error(const std::string& msg) { log(LogLevel::ERROR_L, msg); }

private:
    Logger();                 // Opens log file.
    ~Logger();                // Closes log file.

    std::ofstream logFile_;
    std::mutex    mutex_;

    static std::string currentTimestamp();
    static std::string levelToString(LogLevel level);
};