#include <thales/core/engine.h>
#include <thales/utils/config.h>
#include <thales/utils/logger.h>

#include <iostream>
#include <stdexcept>
#include <string>

int main(int argc, char* argv[]) {
    // Load configuration
    std::string config_path = "config/config.json";
    if (argc > 1) {
        config_path = argv[1];
    }

    thales::utils::Config config;
    if (!config.load_from_file(config_path)) {
        std::cerr << "Failed to load configuration from: " << config_path
                  << std::endl;
        return EXIT_FAILURE;
    }

    // Initialize logger
    bool log_to_file = config.get_bool("logging.log_to_file", true);
    std::string log_file_path =
        config.get_string("logging.log_file_path", "logs/thales.log");
    std::string console_log_level_str =
        config.get_string("logging.consoleLogLevel", "INFO");
    std::string fileLogLevelStr =
        config.get_string("logging.file_log_level", "DEBUG");

    // Convert log level strings to enum values
    auto string_to_log_level =
        [](const std::string& level_str) -> thales::utils::LogLevel {
        if (level_str == "TRACE") return thales::utils::LogLevel::TRACE;
        if (level_str == "DEBUG") return thales::utils::LogLevel::DEBUG;
        if (level_str == "INFO") return thales::utils::LogLevel::INFO;
        if (level_str == "WARNING") return thales::utils::LogLevel::WARNING;
        if (level_str == "ERROR") return thales::utils::LogLevel::ERROR;
        if (level_str == "FATAL") return thales::utils::LogLevel::FATAL;
        return thales::utils::LogLevel::INFO;
    };

    thales::utils::LogLevel console_log_level =
        string_to_log_level(console_log_level_str);
    thales::utils::LogLevel file_log_level =
        string_to_log_level(fileLogLevelStr);

    thales::utils::Logger::initialize(log_to_file, log_file_path,
                                      console_log_level, file_log_level);
    auto& logger = thales::utils::Logger::get_instance();
    logger.info("Trading Bot starting...");

    // Initialize trading engine
    thales::core::Engine engine(config);

    // Start the engine
    if (!engine.initialize()) {
        logger.error("Failed to initialize trading engine");
        return EXIT_FAILURE;
    }

    logger.info("Trading engine initialized successfully");

    // Run the engine
    engine.run();

    return EXIT_SUCCESS;
}
