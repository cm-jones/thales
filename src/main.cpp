#include <iostream>
#include <string>
#include <stdexcept>
#include <thales/core/engine.h>
#include <thales/utils/config.h>
#include <thales/utils/logger.h>

// Include DbLogger header if enabled
#if ENABLE_DB_LOGGER
#include <thales/utils/db_logger.h>
#endif

int main(int argc, char* argv[]) {
    try {
        // Load configuration
        std::string configPath = "config/config.json";
        if (argc > 1) {
            configPath = argv[1];
        }
        
        thales::utils::Config config;
        if (!config.loadFromFile(configPath)) {
            std::cerr << "Failed to load configuration from: " << configPath << std::endl;
            return 1;
        }
        
        // Initialize logger
        bool logToFile = config.getBool("logging.logToFile", true);
        std::string logFilePath = config.getString("logging.logFilePath", "logs/thales.log");
        std::string consoleLogLevelStr = config.getString("logging.consoleLogLevel", "INFO");
        std::string fileLogLevelStr = config.getString("logging.fileLogLevel", "DEBUG");
        
        // Convert log level strings to enum values
        auto stringToLogLevel = [](const std::string& levelStr) -> thales::utils::LogLevel {
            if (levelStr == "TRACE") return thales::utils::LogLevel::TRACE;
            if (levelStr == "DEBUG") return thales::utils::LogLevel::DEBUG;
            if (levelStr == "INFO") return thales::utils::LogLevel::INFO;
            if (levelStr == "WARNING") return thales::utils::LogLevel::WARNING;
            if (levelStr == "ERROR") return thales::utils::LogLevel::ERROR;
            if (levelStr == "FATAL") return thales::utils::LogLevel::FATAL;
            return thales::utils::LogLevel::INFO;
        };
        
        thales::utils::LogLevel consoleLogLevel = stringToLogLevel(consoleLogLevelStr);
        thales::utils::LogLevel fileLogLevel = stringToLogLevel(fileLogLevelStr);
        
        thales::utils::Logger::initialize(logToFile, logFilePath, consoleLogLevel, fileLogLevel);
        auto& logger = thales::utils::Logger::getInstance();
        logger.info("Thales Options Trading Bot starting...");
        
        // Initialize database logger if enabled
#if ENABLE_DB_LOGGER
        bool dbEnabled = config.getBool("logging.database.enabled", false);
        if (dbEnabled) {
            std::string dbHost = config.getString("logging.database.host", "localhost");
            int dbPort = config.getInt("logging.database.port", 5432);
            std::string dbName = config.getString("logging.database.name", "thales");
            std::string dbUser = config.getString("logging.database.user", "thales_user");
            std::string dbPassword = config.getString("logging.database.password", "");
            size_t maxQueueSize = static_cast<size_t>(config.getInt("logging.database.max_queue_size", 10000));
            size_t batchSize = static_cast<size_t>(config.getInt("logging.database.batch_size", 100));
            
            if (thales::utils::DbLogger::initialize(dbHost, dbPort, dbName, dbUser, dbPassword, maxQueueSize, batchSize)) {
                logger.info("Database logger initialized successfully");
            } else {
                logger.error("Failed to initialize database logger");
            }
        }
#endif
        
        logger.info("Configuration loaded successfully");
        
        // Initialize trading engine
        thales::core::Engine engine(config);
        
        // Start the engine
        if (!engine.initialize()) {
            logger.error("Failed to initialize trading engine");
            return 1;
        }
        
        logger.info("Trading engine initialized successfully");
        
        // Run the engine
        engine.run();
        
        logger.info("Thales Options Trading Bot shutting down...");
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred" << std::endl;
        return 1;
    }
}
