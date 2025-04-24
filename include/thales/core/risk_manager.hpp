// SPDX-License-Identifier: MIT

#pragma once

// Standard library includes
#include <string>

// Project includes
#include "thales/core/portfolio.hpp"
#include "thales/utils/config.hpp"

namespace thales {
namespace core {

/// Risk management system for enforcing trading constraints
///
/// Provides real-time risk monitoring and control:
/// - Position size limits
/// - Maximum drawdown tracking
/// - Leverage restrictions
/// - Per-trade risk limits
/// - Daily loss limits
///
/// All risk parameters are configurable and can be updated dynamically
/// based on market conditions and portfolio performance.
class RiskManager {
   public:
    /// Construct a new RiskManager with configuration
    /// @param config Risk management parameters
    explicit RiskManager(const utils::Config &config);

    /// @brief Destructor
    ~RiskManager();

    /// Initialize risk management system
    /// @return true if initialization succeeds, false otherwise
    bool initialize();

    /// Validate order against risk constraints
    /// @param order Order to validate
    /// @param portfolio Current portfolio state
    /// @return true if order complies with risk limits
    bool is_order_allowed(const Order &order, const Portfolio &portfolio) const;

    /// Calculate maximum allowed position size
    /// @param symbol Trading symbol to check
    /// @param portfolio Current portfolio state
    /// @return Maximum allowed position size in base units
    double get_max_position_size(const std::string &symbol,
                                 const Portfolio &portfolio) const;

    /// Calculate current portfolio risk level
    /// @param portfolio Current portfolio state
    /// @return Risk level from 0.0 (lowest) to 1.0 (highest)
    double get_current_risk_level(const Portfolio &portfolio) const;

    /// Update risk parameters dynamically
    /// @param portfolio Current portfolio state to analyze
    void update_risk_params(const Portfolio &portfolio);

   private:
    // Core configuration
    utils::Config config_;  ///< Risk management settings

    // Risk limits
    double max_position_size_;   ///< Maximum position size per instrument
    double max_drawdown_;        ///< Maximum allowed drawdown percentage
    double max_leverage_;        ///< Maximum portfolio leverage ratio
    double max_risk_per_trade_;  ///< Maximum risk per single trade
    double max_daily_loss_;      ///< Maximum allowed daily loss

    /// Calculate risk for a specific position
    /// @param symbol Trading symbol
    /// @param quantity Position size
    /// @param portfolio Current portfolio state
    /// @return Risk measure for the position
    double calculate_position_risk(const std::string &symbol, double quantity,
                                   const Portfolio &portfolio) const;

    /// Calculate aggregate portfolio risk
    /// @param portfolio Current portfolio state
    /// @return Total portfolio risk measure
    double calculate_portfolio_risk(const Portfolio &portfolio) const;
};

}  // namespace core
}  // namespace thales
