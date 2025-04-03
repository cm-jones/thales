#pragma once

#include <fstream>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>

namespace thales {
namespace utils {

/**
 * @enum LogLevel
 * @brief Defines the severity levels for logging.
 */
enum class LogLevel { TRACE, DEBUG, INFO, WARNING, ERROR, FATAL };

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
    static Logger& get_instance();

    /**
     * @brief Initialize the logger
     * @param logToFile Whether to log to a file
     * @param logFilePath Path to the log file
     * @param consoleLevel Minimum level to log to the console
     * @param fileLevel Minimum level to log to the file
     * @return true if initialization was successful, false otherwise
     */
    static bool initialize(bool logToFile = true,
                           const std::string& logFilePath = "logs/thales.log",
                           LogLevel consoleLevel = LogLevel::INFO,
                           LogLevel fileLevel = LogLevel::TRACE);

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
     * @brief Log a trade execution
     * @param strategyName Name of the strategy that generated the trade
     * @param symbol Trading symbol
     * @param orderId Order ID
     * @param executionId Execution ID
     * @param side Trade side (BUY or SELL)
     * @param quantity Trade quantity
     * @param price Trade price
     * @param commission Trade commission
     * @param totalValue Total value of the trade
     * @param executionTime Time of execution
     * @param accountId Account ID
     * @param exchange Exchange name
     * @param orderType Order type
     * @param isOption Whether the trade is for an option
     * @param optionData JSON string with option-specific data
     * @param additionalData JSON string with additional data
     */
    void logTradeExecution(
        const std::string& strategy_name, const std::string& symbol,
        const std::string& order_id, const std::string& execution_id,
        const std::string& side, double quantity, double price,
        double commission, double total_value,
        const std::string& execution_time, const std::string& account_id,
        const std::string& exchange, const std::string& order_type,
        bool is_option = false, const std::string& option_data = "{}",
        const std::string& additional_data = "{}");

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

}  // namespace utils
}  // namespace thales
