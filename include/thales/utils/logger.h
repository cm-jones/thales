#ifndef THALES_UTILS_LOGGER_H
#define THALES_UTILS_LOGGER_H

#include <fstream>
#include <memory>
#include <mutex>
#include <string>
#include <sstream>

namespace thales {
namespace utils {

/**
 * @enum LogLevel
 * @brief Defines the severity levels for logging.
 */
enum class LogLevel {
    TRACE,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL
};

/**
 * @class Logger
 * @brief Singleton logger class for the trading bot.
 * 
 * This class provides thread-safe logging functionality with different
 * severity levels and output destinations (console, file).
 */
class Logger {
public:
    /**
     * @brief Get the singleton instance of the logger
     * @return Reference to the logger instance
     */
    static Logger& getInstance();
    
    /**
     * @brief Initialize the logger
     * @param logToFile Whether to log to a file
     * @param logFilePath Path to the log file
     * @param consoleLevel Minimum level to log to the console
     * @param fileLevel Minimum level to log to the file
     * @return true if initialization was successful, false otherwise
     */
    static bool initialize(
        bool logToFile = true,
        const std::string& logFilePath = "logs/thales.log",
        LogLevel consoleLevel = LogLevel::INFO,
        LogLevel fileLevel = LogLevel::TRACE
    );
    
    /**
     * @brief Log a message with TRACE level
     * @param message The message to log
     */
    void trace(const std::string& message);
    
    /**
     * @brief Log a message with DEBUG level
     * @param message The message to log
     */
    void debug(const std::string& message);
    
    /**
     * @brief Log a message with INFO level
     * @param message The message to log
     */
    void info(const std::string& message);
    
    /**
     * @brief Log a message with WARNING level
     * @param message The message to log
     */
    void warning(const std::string& message);
    
    /**
     * @brief Log a message with ERROR level
     * @param message The message to log
     */
    void error(const std::string& message);
    
    /**
     * @brief Log a message with FATAL level
     * @param message The message to log
     */
    void fatal(const std::string& message);
    
    /**
     * @brief Set the minimum log level for console output
     * @param level The minimum log level
     */
    void setConsoleLevel(LogLevel level);
    
    /**
     * @brief Set the minimum log level for file output
     * @param level The minimum log level
     */
    void setFileLevel(LogLevel level);
    
    /**
     * @brief Enable or disable file logging
     * @param enable Whether to enable file logging
     * @param logFilePath Path to the log file (if enabling)
     * @return true if the operation was successful, false otherwise
     */
    bool setFileLogging(bool enable, const std::string& logFilePath = "");
    
private:
    // Private constructor for singleton pattern
    Logger();
    
    // Prevent copying and assignment
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Log a message with the specified level
    void log(LogLevel level, const std::string& message);
    
    // Convert a log level to a string
    static std::string levelToString(LogLevel level);
    
    // Get the current timestamp as a string
    static std::string getCurrentTimestamp();
    
    // Instance variables
    std::mutex mutex_;
    bool logToFile_;
    std::string logFilePath_;
    LogLevel consoleLevel_;
    LogLevel fileLevel_;
    std::unique_ptr<std::ofstream> logFile_;
    
    // Singleton instance
    static std::unique_ptr<Logger> instance_;
};

} // namespace utils
} // namespace thales

#endif // THALES_UTILS_LOGGER_H
