#include <unistd.h>

#include <cstdlib>
#include <iostream>
#include <string>
#include <thales/core/engine.hpp>
#include <thales/data/ib_client.hpp>
#include <thales/utils/config.hpp>
#include <thales/utils/logger.hpp>
#include <thales/utils/symbol_lookup.hpp>
#include <vector>

int main(int argc, char** argv) {
    long num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    std::cout << "Number of available cores: " << num_cores << "\n";

    // Load configuration
    std::string config_path = "config/config.json";
    
    // Parse command line arguments for --config flag
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--config" && i + 1 < argc) {
            config_path = argv[i + 1];
            break;
        } else if (arg.starts_with("--config=")) {
            config_path = arg.substr(9); // Extract path after --config=
            break;
        }
    }

    thales::utils::Config config;
    if (!config.load_from_file(config_path)) {
        std::cerr << "Failed to load configuration from: " << config_path << "\n";
        return EXIT_FAILURE;
    }

    // Extract symbols from config
    std::vector<std::string> symbols = config.get_string_vector("data.symbols");
    if (symbols.empty()) {
        std::cerr << "No symbols found in configuration" << "\n";
        return EXIT_FAILURE;
    }

    // Initialize symbol lookup table
    thales::utils::SymbolLookup::initialize(symbols);

    // Initialize logger
    bool log_to_file = config.get_bool("logging.log_to_file", true);
    std::string log_file_path =
        config.get_string("logging.log_file_path", "logs/thales.log");
    std::string console_log_level_str =
        config.get_string("logging.console_log_level", "INFO");
    std::string file_log_level_str =
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
        string_to_log_level(file_log_level_str);

    thales::utils::Logger::initialize(log_to_file, log_file_path,
                                      console_log_level, file_log_level);
    auto& logger = thales::utils::Logger::get_instance();
    logger.info("Trading Bot starting...");

    // Instantiate the IBClient
    thales::data::IBClient ib_client(config);

    // Set up a simple callback to log market data updates
    ib_client.setMarketDataCallback(
        [&logger](const thales::data::MarketData& data) {
            logger.info("Market data received: " + data.symbol +
                        " - Price: " + std::to_string(data.price));
        });

    // Connect to IB Gateway
    if (!ib_client.connect()) {
        logger.error("Failed to connect to Interactive Brokers");
        return EXIT_FAILURE;
    }

    logger.info("Successfully connected to Interactive Brokers");

    for (const auto& symbol : symbols) {
        logger.info("Subscribing to symbol: " + symbol);
        if (!ib_client.subscribe_market_data(symbol)) {
            logger.error("Failed to subscribe to market data for " + symbol);
            return EXIT_FAILURE;
        }

        auto market_data = ib_client.get_latest_market_data(symbol);
        logger.info("Latest price for " + symbol + ": " +
            std::to_string(market_data.price));
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

    ib_client.disconnect();
    logger.info("Disconnected from Interactive Brokers");

    return EXIT_SUCCESS;
}
