#include <unistd.h>

#include <print>
#include <stdexcept>
#include <string>
#include <thales/core/engine.hpp>
#include <thales/data/ib_client.hpp>
#include <thales/utils/config.hpp>
#include <thales/utils/logger.hpp>

int main(int argc, char** argv) {
    int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    std::println("Number of available cores: {}", std::to_string(num_cores));

    // Load configuration
    std::string config_path = "config/config.json";
    if (argc > 1) {
        config_path = argv[1];
    }

    thales::utils::Config config;
    if (!config.load_from_file(config_path)) {
        std::println(stderr, "Failed to load configuration from: {}",
                     config_path);
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

    // Test IB connection directly
    logger.info("Testing connection to Interactive Brokers...");
    thales::data::IBClient ib_client(config);

    // Set up a simple callback to log market data updates
    ib_client.setMarketDataCallback(
        [&logger](const thales::data::MarketData& data) {
            logger.info("Market data received: " + data.symbol +
                        " - Price: " + std::to_string(data.price));
        });

    // Connect to IB Gateway
    if (ib_client.connect()) {
        logger.info("Successfully connected to Interactive Brokers");

        // Subscribe to a test symbol
        std::string test_symbol = "SPY";
        if (ib_client.subscribeMarketData(test_symbol)) {
            logger.info("Successfully subscribed to " + test_symbol);

            // Wait a moment to receive some data
            logger.info("Waiting for market data...");
            sleep(3);

            // Get the latest market data
            auto market_data = ib_client.getLatestMarketData(test_symbol);
            logger.info("Latest price for " + test_symbol + ": " +
                        std::to_string(market_data.price));
        }

        // Disconnect when done testing
        ib_client.disconnect();
        logger.info("Disconnected from Interactive Brokers");
    } else {
        logger.error("Failed to connect to Interactive Brokers");
        // Continue anyway, as the engine will try to connect again
    }

    // Instantiate trading engine
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
