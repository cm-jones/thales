#pragma once

#include <memory>
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
    bool isOrderAllowed(const Order& order, const Portfolio& portfolio) const;

    /**
     * @brief Get the maximum allowed position size for a symbol
     * @param symbol The symbol to check
     * @param portfolio The current portfolio
     * @return The maximum allowed position size
     */
    double getMaxPositionSize(const std::string& symbol,
                              const Portfolio& portfolio) const;

    /**
     * @brief Get the current risk level
     * @param portfolio The current portfolio
     * @return The current risk level (0.0 to 1.0, where 1.0 is maximum risk)
     */
    double getCurrentRiskLevel(const Portfolio& portfolio) const;

    /**
     * @brief Update risk parameters based on market conditions
     * @param portfolio The current portfolio
     */
    void updateRiskParameters(const Portfolio& portfolio);

   private:
    // Configuration
    utils::Config config_;

    // Risk parameters
    double maxPositionSize_;
    double maxDrawdown_;
    double maxLeverage_;
    double maxRiskPerTrade_;
    double maxDailyLoss_;

    // Private methods
    double calculatePositionRisk(const std::string& symbol, double quantity,
                                 const Portfolio& portfolio) const;
    double calculatePortfolioRisk(const Portfolio& portfolio) const;
};

}  // namespace core
}  // namespace thales
