#include <thales/core/engine.h>
#include <thales/utils/logger.h>
#include <chrono>
#include <thread>

namespace thales {
namespace core {

Engine::Engine(const utils::Config& config)
    : config_(config), running_(false) {
}

Engine::~Engine() {
    stop();
}

bool Engine::initialize() {
    auto& logger = utils::Logger::getInstance();
    logger.info("Initializing trading engine...");
    
    try {
        // Initialize strategy registry
        logger.info("Initializing strategy registry...");
        strategyRegistry_ = std::make_unique<strategies::StrategyRegistry>(config_);
        if (!strategyRegistry_->initialize()) {
            logger.error("Failed to initialize strategy registry");
            return false;
        }
        
        logger.info("Trading engine initialized successfully");
        return true;
    } catch (const std::exception& e) {
        logger.error("Exception during engine initialization: " + std::string(e.what()));
        return false;
    } catch (...) {
        logger.error("Unknown exception during engine initialization");
        return false;
    }
}

void Engine::run() {
    auto& logger = utils::Logger::getInstance();
    logger.info("Starting trading engine...");
    
    running_ = true;
    
    try {
        mainLoop();
    } catch (const std::exception& e) {
        logger.error("Exception in main loop: " + std::string(e.what()));
    } catch (...) {
        logger.error("Unknown exception in main loop");
    }
    
    running_ = false;
    logger.info("Trading engine stopped");
}

void Engine::stop() {
    if (running_) {
        auto& logger = utils::Logger::getInstance();
        logger.info("Stopping trading engine...");
        running_ = false;
    }
}

bool Engine::isRunning() const {
    return running_;
}

void Engine::mainLoop() {
    auto& logger = utils::Logger::getInstance();
    
    // Get the loop interval from configuration
    int loopIntervalMs = config_.getInt("engine.loopIntervalMs", 1000);
    
    logger.info("Main loop started with interval: " + std::to_string(loopIntervalMs) + "ms");
    
    while (running_) {
        // Process signals from strategies
        processSignals();
        
        // Execute orders
        executeOrders();
        
        // Update portfolio
        updatePortfolio();
        
        // Sleep for the configured interval
        std::this_thread::sleep_for(std::chrono::milliseconds(loopIntervalMs));
    }
}

void Engine::processSignals() {
    // Execute strategies to generate signals
    if (strategyRegistry_) {
        strategyRegistry_->executeStrategies();
    }
}

void Engine::executeOrders() {
    // Execute orders based on signals and risk constraints
    // This is a placeholder for the actual implementation
}

void Engine::updatePortfolio() {
    // Update portfolio with latest market data
    // This is a placeholder for the actual implementation
}

} // namespace core
} // namespace thales
