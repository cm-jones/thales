// SPDX-License-Identifier: MIT

#pragma once

#include <memory>
#include <string>
#include <thales/core/portfolio.hpp>
#include <thales/data/data_manager.hpp>
#include <thales/strategies/strategy_base.hpp>
#include <thales/utils/config.hpp>
#include <unordered_map>
#include <vector>

namespace thales {
namespace strategies {

/// @class StrategyRegistry
/// @brief Registry for trading strategies.
/// @note This class manages the registration, initialization, and execution
/// of trading strategies.
class StrategyRegistry {
  public:
    /// @brief Constructor
    /// @param config The configuration for the strategy registry
    /// @brief Constructor
    /// @param config The configuration for the strategy registry
    /// @param data_manager Shared pointer to the data manager
    /// @param portfolio Raw pointer to the portfolio (owned by Engine)
    explicit StrategyRegistry(const utils::Config &config,
                              std::shared_ptr<data::DataManager> &&data_manager,
                              core::Portfolio *portfolio);

    /// @brief Destructor
    ~StrategyRegistry() = default;

    /// @brief Initialize the strategy registry
    /// @return true if initialization was successful, false otherwise
    bool initialize();

    /// @brief Register a strategy
    /// @param strategy The strategy to register
    /// @return true if registration was successful, false otherwise
    bool register_strategy(std::unique_ptr<StrategyBase> strategy);

    /// @brief Get a strategy by name
    /// @param name The name of the strategy to get
    /// @return A pointer to the strategy, or nullptr if not found
    StrategyBase *get_strategy(const std::string &name) const;

    /// @brief Get all registered strategies
    /// @return A vector of pointers to all registered strategies
    std::vector<StrategyBase *> get_all_strategies() const;

    /// @brief Execute all registered strategies
    /// @return true if execution was successful, false otherwise
    bool execute_strategies();

    /// @brief Enable a strategy
    /// @param name The name of the strategy to enable
    /// @return true if the strategy was enabled, false otherwise
    bool enable_strategy(const std::string &name);

    /// @brief Disable a strategy
    /// @param name The name of the strategy to disable
    /// @return true if the strategy was disabled, false otherwise
    bool disable_strategy(const std::string &name);

    /// @brief Check if a strategy is enabled
    /// @param name The name of the strategy to check
    /// @return true if the strategy is enabled, false otherwise
    bool is_strategy_enabled(const std::string &name) const;

    /// @brief Get the data manager shared pointer
    /// @return Shared pointer to the data manager
    std::shared_ptr<data::DataManager> get_data_manager() const {
        return data_manager_;
    }

  private:
    // Core components
    utils::Config config_;
    std::shared_ptr<data::DataManager> data_manager_;
    core::Portfolio *portfolio_; // Non-owning pointer

    // Strategy management
    std::unordered_map<std::string, std::unique_ptr<StrategyBase>> strategies_;
    std::unordered_map<std::string, bool> enabled_strategies_;

    // Private methods
    void load_strategies_from_config();
};

} // namespace strategies
} // namespace thales
