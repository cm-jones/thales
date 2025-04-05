#pragma once

#include <string>
#include <thales/core/portfolio.hpp>
#include <thales/utils/config.hpp>

namespace thales {
namespace core {

/**
 * @class RiskManager
 * @brief Manages risk for the trading bot.
 *
 * This class is responsible for enforcing risk limits and constraints
 * on trading activities, such as maximum position sizes, maximum
 * drawdown, and other risk metrics.
 */
class RiskManager {
   public:
    /**
     * @brief Constructor
     * @param config The configuration for the risk manager
     */
    explicit RiskManager(const utils::Config& config);

    /**
     * @brief Destructor
     */
    ~RiskManager();

    /**
     * @brief Initialize the risk manager
     * @return true if initialization was successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Check if an order is allowed based on risk constraints
     * @param order The order to check
     * @param portfolio The current portfolio
     * @return true if the order is allowed, false otherwise
     */
    bool is_order_allowed(const Order& order, const Portfolio& portfolio) const;

    /**
     * @brief Get the maximum allowed position size for a symbol
     * @param symbol The symbol to check
     * @param portfolio The current portfolio
     * @return The maximum allowed position size
     */
    double get_max_position_size(const std::string& symbol,
                                 const Portfolio& portfolio) const;

    /**
     * @brief Get the current risk level
     * @param portfolio The current portfolio
     * @return The current risk level (0.0 to 1.0, where 1.0 is maximum risk)
     */
    double get_current_risk_level(const Portfolio& portfolio) const;

    /**
     * @brief Update risk parameters based on market conditions
     * @param portfolio The current portfolio
     */
    void update_risk_params(const Portfolio& portfolio);

   private:
    // Configuration
    utils::Config config_;  // Configuration object (varies, likely >32 bytes)

    // Risk parameters
    double max_position_size_;   // Maximum position size (8 bytes)
    double max_drawdown_;        // Maximum drawdown (8 bytes)
    double max_leverage_;        // Maximum leverage (8 bytes)
    double max_risk_per_trade_;  // Maximum risk per trade (8 bytes)
    double max_daily_loss_;      // Maximum daily loss (8 bytes)

    // Private methods
    double calculate_position_risk(const std::string& symbol, double quantity,
                                   const Portfolio& portfolio) const;
    double calculate_portfolio_risk(const Portfolio& portfolio) const;
};

}  // namespace core
}  // namespace thales
