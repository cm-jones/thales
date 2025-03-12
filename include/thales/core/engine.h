#ifndef THALES_CORE_ENGINE_H
#define THALES_CORE_ENGINE_H

#include <atomic>
#include <memory>
#include <string>
#include <thales/utils/config.h>
#include <thales/strategies/strategy_registry.h>

namespace thales {
namespace core {

/**
 * @class Engine
 * @brief The main trading engine that orchestrates all components of the system.
 * 
 * The Engine class is responsible for initializing and coordinating all the components
 * of the trading system, including data management, strategy execution, risk management,
 * and portfolio management.
 */
class Engine {
public:
    /**
     * @brief Constructor
     * @param config The configuration for the trading engine
     */
    explicit Engine(const utils::Config& config);
    
    /**
     * @brief Destructor
     */
    ~Engine();
    
    /**
     * @brief Initialize the trading engine
     * @return true if initialization was successful, false otherwise
     */
    bool initialize();
    
    /**
     * @brief Run the trading engine
     * 
     * This method starts the main trading loop and blocks until the engine is stopped.
     */
    void run();
    
    /**
     * @brief Stop the trading engine
     * 
     * This method signals the engine to stop and return from the run() method.
     */
    void stop();
    
    /**
     * @brief Check if the engine is running
     * @return true if the engine is running, false otherwise
     */
    bool isRunning() const;

private:
    // Configuration
    utils::Config config_;
    
    // Components
    std::unique_ptr<strategies::StrategyRegistry> strategyRegistry_;
    
    // Engine state
    std::atomic<bool> running_;
    
    // Private methods
    void mainLoop();
    void processSignals();
    void executeOrders();
    void updatePortfolio();
};

} // namespace core
} // namespace thales

#endif // THALES_CORE_ENGINE_H
};

} // namespace core
} // namespace thales

#endif // THALES_CORE_ENGINE_H
