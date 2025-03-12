#include <iostream>
#include <string>
#include <stdexcept>
#include <thales/core/engine.h>
#include <thales/utils/config.h>
#include <thales/utils/logger.h>

int main(int argc, char* argv[]) {
    try {
        // Initialize logger
        thales::utils::Logger::initialize();
        auto& logger = thales::utils::Logger::getInstance();
        logger.info("Thales Options Trading Bot starting...");
        
        // Load configuration
        std::string configPath = "config/config.json";
        if (argc > 1) {
            configPath = argv[1];
        }
        
        thales::utils::Config config;
        if (!config.loadFromFile(configPath)) {
            logger.error("Failed to load configuration from: " + configPath);
            return 1;
        }
        
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
