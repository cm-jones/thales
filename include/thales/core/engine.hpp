#pragma once

#include <atomic>
#include <memory>
#include <thales/core/portfolio.hpp>
#include <thales/data/data_manager.hpp>
#include <thales/strategies/strategy_registry.hpp>
#include <thales/utils/config.hpp>

namespace thales {
namespace core {

/**
 * @class Engine
 * @brief The main trading engine that orchestrates all components of the
 * system.
 *
 * The Engine class is responsible for initializing and coordinating all the
 * components of the trading system, including data management, strategy
 * execution, risk management, and portfolio management.
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
     * This method starts the main trading loop and blocks until the engine
     * is stopped.
     */
    void run();

    /**
     * @brief Stop the trading engine
     *
     * This method signals the engine to stop and return from the run()
     * method.
     */
    void stop();

    /**
     * @brief Check if the engine is running
     * @return true if the engine is running, false otherwise
     */
    bool is_running() const;

   private:
    // Configuration
    utils::Config config_;  // Configuration object (varies, likely >32 bytes)

    // Components
    std::unique_ptr<data::DataManager> data_manager_;  // Data manager (8 bytes)
    std::unique_ptr<Portfolio> portfolio_;             // Portfolio (8 bytes)
    std::unique_ptr<strategies::StrategyRegistry>
        strategy_registry_;  // Strategy registry (8 bytes)

    // Engine state
    std::atomic<bool> running_;  // Running flag (1-8 bytes)

    // Private methods
    void main_loop();
    void process_signals();
    void execute_orders();
    void update_portfolio();
};

}  // namespace core
}  // namespace thales
