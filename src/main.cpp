// SPDX-License-Identifier: MIT

#include <iostream>
#include <string>

#include "thales/core/engine.hpp"
#include "thales/data/ib_client.hpp"
#include "thales/utils/config.hpp"
#include "thales/utils/logger.hpp"
#include "thales/utils/symbol_lookup.hpp"

using namespace thales::utils;
using namespace thales::core;
using namespace thales::data;

int main(int argc, char *argv[]) {
    std::string config_path;

    switch (argc) {
        case 1: {
            // No arguments, use default config path
            config_path = "config/config.json";
            break;
        }

        case 2: {
            // One argument, check if it's a flag
            if (std::string(argv[1]) == "--config") {
                std::cerr << "Error: --config requires a path argument\n";
                return EXIT_FAILURE;
            } else {
                config_path = argv[1];
            }
            break;
        }

        default:
            // Multiple arguments, check for --config flag
            config_path = "config/config.json";  // Default path
            break;
    }

    // Create config object
    Config config;
    if (!config.load_from_file(config_path)) {
        std::cerr << "Failed to load configuration from: " << config_path
                  << '\n';
        return EXIT_FAILURE;
    }

    // Extract symbols from config
    std::vector<std::string> symbols = config.get_string_vector("data.symbols");
    if (symbols.empty()) {
        std::cerr << "No stock tickers found in configuration file: "
                  << config_path << '\n';
        return EXIT_FAILURE;
    }

    // Initialize symbol lookup table
    SymbolLookup::initialize(symbols);

    // Initialize logger
    bool log_to_file = config.get_bool("logging.log_to_file", true);
    std::string log_file_path =
        config.get_string("logging.log_file_path", "logs/thales.log");
    std::string console_log_level_str =
        config.get_string("logging.console_log_level", "INFO");
    std::string file_log_level_str =
        config.get_string("logging.file_log_level", "DEBUG");

    // Convert log level strings to enum values
    auto string_to_log_level = [](const std::string &level_str) -> LogLevel {
        if (level_str == "TRACE") return LogLevel::TRACE;
        if (level_str == "DEBUG") return LogLevel::DEBUG;
        if (level_str == "INFO") return LogLevel::INFO;
        if (level_str == "WARNING") return LogLevel::WARNING;
        if (level_str == "ERROR") return LogLevel::ERROR;
        if (level_str == "FATAL") return LogLevel::FATAL;
        return LogLevel::INFO;
    };

    LogLevel console_log_level = string_to_log_level(console_log_level_str);
    LogLevel file_log_level = string_to_log_level(file_log_level_str);

    Logger::initialize(log_to_file, log_file_path, console_log_level,
                       file_log_level);
    auto &logger = Logger::get_instance();
    logger.info("Trading Bot starting...");

    // Instantiate the IBClient
    IBClient ib_client(config);

    // Set up a simple callback to log market data updates
    ib_client.set_market_data_callback(
        [&logger](const thales::data::MarketData &data) {
            logger.info("Market data received: " + data.symbol +
                        " - Price: " + std::to_string(data.price));
        });

    // Connect to IB Gateway
    if (!ib_client.connect()) {
        logger.error("Failed to connect to Interactive Brokers");
        return EXIT_FAILURE;
    }

    logger.info("Successfully connected to Interactive Brokers");

    for (const auto &symbol : symbols) {
        logger.info("Subscribing to symbol: " + symbol);
        if (!ib_client.subscribe_market_data(symbol)) {
            logger.error("Failed to subscribe to market data for " + symbol);
            return EXIT_FAILURE;
        }

        auto market_data = ib_client.get_latest_market_data(symbol);
        logger.info("Latest price for " + symbol + ": " +
                    std::to_string(market_data.price));
    }

    // Initialize trading engine
    Engine engine(config);
    if (!engine.initialize()) {
        logger.error("Failed to initialize trading engine");
        return EXIT_FAILURE;
    }

    logger.info("Trading engine initialized successfully");

    // Run the engine
    engine.run();

    // If we reach here, the engine has finished running
    ib_client.disconnect();

    logger.info("Disconnected from Interactive Brokers");

    return EXIT_SUCCESS;
}
