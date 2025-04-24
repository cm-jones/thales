// SPDX-License-Identifier: MIT

#pragma once

// Standard library includes
#include <atomic>
#include <memory>

// Project includes
#include <thales/core/portfolio.hpp>
#include <thales/data/data_manager.hpp>
#include <thales/strategies/strategy_registry.hpp>
#include <thales/utils/config.hpp>

namespace thales {
namespace core {

/// Main trading engine that orchestrates all components of the system.
///
/// The Engine class is responsible for initializing and coordinating all core
/// components:
/// - Data management (market data, historical data)
/// - Strategy execution and signal processing
/// - Portfolio management and position tracking
/// - Risk management and exposure control
///
/// The engine operates in a main loop that processes market data, executes
/// trading strategies, and manages order flow while ensuring proper
/// synchronization between components.
class Engine {
  public:
    /// Constructor for the Engine class
    /// @param config Configuration parameters for the trading engine
    explicit Engine(const utils::Config &config);

    /// @brief Destructor
    virtual ~Engine();

    /// Initialize all engine components
    ///
    /// Initializes the data manager, portfolio, and strategy registry in order.
    /// Each component must initialize successfully for the engine to start.
    ///
    /// @return true if all components initialized successfully, false otherwise
    bool initialize();

    /// Run the trading engine main loop
    ///
    /// Starts the main trading loop and blocks until the engine is stopped.
    /// Processes market data, executes strategies, and manages order flow.
    void run();

    /// Stop the trading engine
    ///
    /// Signals the engine to stop and return from the run() method.
    /// Ensures clean shutdown of all components.
    void stop();

    /// Check if the engine is running
    /// @return true if the engine is running, false otherwise
    bool is_running() const;

  private:
    // Core configuration
    utils::Config config_; ///< Engine configuration parameters

    // Primary components
    std::shared_ptr<data::DataManager>
        data_manager_; ///< Market data management (shared for potential
                       ///< multi-engine scenarios)
    std::unique_ptr<Portfolio>
        portfolio_; ///< Portfolio tracking (unique per engine)
    std::unique_ptr<strategies::StrategyRegistry> ///< Strategy management
                                                  ///< (unique per engine)
                                                      strategy_registry_;

    // Engine state with atomic operations for thread safety
    std::atomic<bool> running_{false}; ///< Flag indicating engine running state

    // Core engine operations (all thread-safe)
    /// Main processing loop for the engine
    void main_loop();

    /// Process strategy signals and generate orders (synchronized)
    void process_signals();

    /// Execute pending orders in the system (synchronized)
    void execute_orders();

    /// Update portfolio positions and values (synchronized)
    void update_portfolio();
};

} // namespace core
} // namespace thales
