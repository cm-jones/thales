// SPDX-License-Identifier: MIT

#include "thales/utils/logger.hpp"

#include <chrono>
#include <ctime>
#include <filesystem>
#include <iomanip>
#include <iostream>

// Forward declaration for DbLogger
namespace thales {
namespace utils {
class DbLogger;
}
}  // namespace thales

// Include DbLogger header if enabled
#if ENABLE_DB_LOGGER
#include "thales/utils/db_logger.h>
#endif

namespace thales {
namespace utils {

// Initialize the static instance
std::unique_ptr<Logger> Logger::instance_ = nullptr;

Logger::Logger()
    : logToFile_(false),
      log_file_path_("logs/thales.log"),
      console_level_(LogLevel::INFO),
      file_level_(LogLevel::TRACE) {}

Logger &Logger::get_instance() {
    if (!instance_) {
        initialize();
    }

    return *instance_;
}

bool Logger::initialize(bool log_to_file, const std::string &log_file_path,
                        LogLevel console_level, LogLevel file_level) {
    if (!instance_) {
        instance_ = std::unique_ptr<Logger>(new Logger());
    }

    instance_->console_level_ = console_level;
    instance_->file_level_ = file_level;

    return instance_->set_file_logging(log_to_file, log_file_path);
}

void Logger::trace(const std::string &message) {
    log(LogLevel::TRACE, message);
}

void Logger::debug(const std::string &message) {
    log(LogLevel::DEBUG, message);
}

void Logger::info(const std::string &message) { log(LogLevel::INFO, message); }

void Logger::warning(const std::string &message) {
    log(LogLevel::WARNING, message);
}

void Logger::error(const std::string &message) {
    log(LogLevel::ERROR, message);
}

void Logger::fatal(const std::string &message) {
    log(LogLevel::FATAL, message);
}

void Logger::log_trade_execution(
    const std::string &strategy_name, const std::string &symbol,
    const std::string &order_id, const std::string &execution_id,
    const std::string &side, double quantity, double price, double commission,
    double total_value, const std::string &execution_time,
    const std::string &account_id, const std::string &exchange,
    const std::string &order_type, bool is_option,
    const std::string &option_data, const std::string &additional_data) {
    // Log to console and file
    std::stringstream ss;
    ss << "Trade execution: " << strategy_name << ", " << symbol << ", " << side
       << ", " << quantity << " @ " << price << ", total: " << total_value;
    info(ss.str());

    // Log to database if enabled
#if ENABLE_DB_LOGGER
    auto &dbLogger = DbLogger::get_instance();
    bool success = dbLogger.log_trade_execution(
        strategy_name, symbol, order_id, execution_id, side, quantity, price,
        commission, total_value, execution_time, account_id, exchange,
        order_type, is_option, option_data, additional_data);

    if (!success) {
        error("Failed to log trade execution to database");
    }
#endif
}

void Logger::set_console_level(LogLevel level) {
    std::lock_guard<std::mutex> lock(mutex_);
    console_level_ = level;
}

void Logger::set_file_level(LogLevel level) {
    std::lock_guard<std::mutex> lock(mutex_);
    file_level_ = level;
}

bool Logger::set_file_logging(bool enable, const std::string &log_file_path) {
    std::lock_guard<std::mutex> lock(mutex_);

    // Close the current log file if it's open
    if (log_file_) {
        log_file_->close();
        log_file_.reset();
    }

    logToFile_ = enable;

    if (!enable) {
        return true;
    }

    // Update the log file path if provided
    if (!log_file_path.empty()) {
        log_file_path_ = log_file_path;
    }

    // Create the directory if it doesn't exist
    std::filesystem::path path(log_file_path_);
    std::filesystem::create_directories(path.parent_path());

    // Open the log file
    log_file_ = std::make_unique<std::ofstream>(log_file_path_, std::ios::app);

    if (!log_file_->is_open()) {
        std::cerr << "Failed to open log file: " << log_file_path_ << '\n';
        logToFile_ = false;
        return false;
    }

    return true;
}

void Logger::log(LogLevel level, const std::string &message) {
    std::lock_guard<std::mutex> lock(mutex_);

    std::string timestamp = get_current_timestamp();
    std::string levelStr = levelToString(level);

    // Log to console if the level is high enough
    if (level >= console_level_) {
        std::cout << timestamp << " [" << levelStr << "] " << message << '\n';
    }

    // Log to file if enabled and the level is high enough
    if (logToFile_ && log_file_ && level >= file_level_) {
        *log_file_ << timestamp << " [" << levelStr << "] " << message << '\n';
        log_file_->flush();
    }
}

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::TRACE:
            return "TRACE";
        case LogLevel::DEBUG:
            return "DEBUG";
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::WARNING:
            return "WARNING";
        case LogLevel::ERROR:
            return "ERROR";
        case LogLevel::FATAL:
            return "FATAL";
        default:
            return "UNKNOWN";
    }
}

std::string Logger::get_current_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                  now.time_since_epoch()) %
              1000;

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();

    return ss.str();
}

}  // namespace utils
}  // namespace thales
