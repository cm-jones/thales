#include <thales/utils/logger.h>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <filesystem>

namespace thales {
namespace utils {

// Initialize the static instance
std::unique_ptr<Logger> Logger::instance_ = nullptr;

Logger::Logger()
    : logToFile_(false),
      logFilePath_("logs/thales.log"),
      consoleLevel_(LogLevel::INFO),
      fileLevel_(LogLevel::TRACE) {
}

Logger& Logger::getInstance() {
    if (!instance_) {
        initialize();
    }
    return *instance_;
}

bool Logger::initialize(
    bool logToFile,
    const std::string& logFilePath,
    LogLevel consoleLevel,
    LogLevel fileLevel
) {
    if (!instance_) {
        instance_ = std::unique_ptr<Logger>(new Logger());
    }
    
    instance_->consoleLevel_ = consoleLevel;
    instance_->fileLevel_ = fileLevel;
    
    return instance_->setFileLogging(logToFile, logFilePath);
}

void Logger::trace(const std::string& message) {
    log(LogLevel::TRACE, message);
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

void Logger::fatal(const std::string& message) {
    log(LogLevel::FATAL, message);
}

void Logger::setConsoleLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(mutex_);
    consoleLevel_ = level;
}

void Logger::setFileLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(mutex_);
    fileLevel_ = level;
}

bool Logger::setFileLogging(bool enable, const std::string& logFilePath) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Close the current log file if it's open
    if (logFile_) {
        logFile_->close();
        logFile_.reset();
    }
    
    logToFile_ = enable;
    
    if (!enable) {
        return true;
    }
    
    // Update the log file path if provided
    if (!logFilePath.empty()) {
        logFilePath_ = logFilePath;
    }
    
    // Create the directory if it doesn't exist
    std::filesystem::path path(logFilePath_);
    std::filesystem::create_directories(path.parent_path());
    
    // Open the log file
    logFile_ = std::make_unique<std::ofstream>(logFilePath_, std::ios::app);
    
    if (!logFile_->is_open()) {
        std::cerr << "Failed to open log file: " << logFilePath_ << std::endl;
        logToFile_ = false;
        return false;
    }
    
    return true;
}

void Logger::log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::string timestamp = getCurrentTimestamp();
    std::string levelStr = levelToString(level);
    
    // Log to console if the level is high enough
    if (level >= consoleLevel_) {
        std::cout << timestamp << " [" << levelStr << "] " << message << std::endl;
    }
    
    // Log to file if enabled and the level is high enough
    if (logToFile_ && logFile_ && level >= fileLevel_) {
        *logFile_ << timestamp << " [" << levelStr << "] " << message << std::endl;
        logFile_->flush();
    }
}

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::TRACE:   return "TRACE";
        case LogLevel::DEBUG:   return "DEBUG";
        case LogLevel::INFO:    return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR:   return "ERROR";
        case LogLevel::FATAL:   return "FATAL";
        default:                return "UNKNOWN";
    }
}

std::string Logger::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()
    ) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    
    return ss.str();
}

} // namespace utils
} // namespace thales
