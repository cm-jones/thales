// SPDX-License-Identifier: MIT

// Standard library includes
#include <cmath>

// Project includes
#include "thales/core/risk_manager.hpp"
#include "thales/utils/logger.hpp"

namespace thales {
namespace core {

RiskManager::RiskManager(const utils::Config &config)
    : config_(config),
      max_position_size_(config.get_double("risk.maxPositionSize", 100000.0)),
      max_drawdown_(config.get_double("risk.maxDrawdown", 0.10)),
      max_leverage_(config.get_double("risk.maxLeverage", 2.0)),
      max_risk_per_trade_(config.get_double("risk.maxRiskPerTrade", 0.02)),
      max_daily_loss_(config.get_double("risk.maxDailyLoss", 5000.0)) {}

RiskManager::~RiskManager() = default;

bool RiskManager::initialize() {
    try {
        auto &logger = utils::Logger::get_instance();
        logger.info("Initializing risk management system...");

        // Validate risk parameters
        if (max_position_size_ <= 0.0 || max_drawdown_ <= 0.0 ||
            max_leverage_ <= 0.0 || max_risk_per_trade_ <= 0.0 ||
            max_daily_loss_ <= 0.0) {
            logger.error("Invalid risk parameters in configuration");
            return false;
        }

        logger.info("Risk management system initialized successfully");
        return true;
    } catch (const std::exception &e) {
        auto &logger = utils::Logger::get_instance();
        logger.error("Failed to initialize risk management: " +
                     std::string(e.what()));
        return false;
    }
}

bool RiskManager::is_order_allowed(const Order &order,
                                   const Portfolio &portfolio) const {
    try {
        // Check position size limit
        const auto current_pos = portfolio.get_position(
            utils::SymbolLookup::get_instance().get_symbol(order.symbol_id));

        double new_size = current_pos.quantity;
        if (order.side == Order::Side::BUY) {
            new_size += order.quantity;
        } else {
            new_size -= order.quantity;
        }

        if (std::abs(new_size) > max_position_size_) {
            return false;
        }

        // Check risk per trade
        const double order_risk = calculate_position_risk(
            utils::SymbolLookup::get_instance().get_symbol(order.symbol_id),
            order.quantity, portfolio);

        if (order_risk > max_risk_per_trade_) {
            return false;
        }

        // Check portfolio risk
        const double total_risk = calculate_portfolio_risk(portfolio);
        if (total_risk > max_leverage_) {
            return false;
        }

        return true;
    } catch (const std::exception &e) {
        auto &logger = utils::Logger::get_instance();
        logger.error("Error validating order: " + std::string(e.what()));
        return false;
    }
}

double RiskManager::get_max_position_size(const std::string &symbol,
                                          const Portfolio &portfolio) const {
    try {
        // First check if we already have a position in this symbol
        const auto existing_pos = portfolio.get_position(symbol);
        const double portfolio_value = portfolio.get_total_value();
        if (portfolio_value <= 0.0) {
            return 0.0;
        }

        // Start with base position limit
        double max_size = max_position_size_;

        // Adjust for existing position
        if (existing_pos.quantity > 0) {
            // If we already have a position, reduce available size
            max_size = std::max(0.0, max_size - existing_pos.quantity);
        }

        // Apply portfolio-based limits
        if (portfolio_value > 0.0) {
            const double concentration_limit =
                portfolio_value * max_risk_per_trade_;
            max_size = std::min(max_size, concentration_limit);
        }

        // Adjust for current risk level
        const double risk_adjustment = 1.0 - get_current_risk_level(portfolio);
        max_size *= risk_adjustment;

        return std::max(0.0, max_size);
    } catch (const std::exception &e) {
        auto &logger = utils::Logger::get_instance();
        logger.error("Error calculating max position size: " +
                     std::string(e.what()));
        return 0.0;
    }
}

double RiskManager::get_current_risk_level(const Portfolio &portfolio) const {
    try {
        const double total_value = portfolio.get_total_value();
        if (total_value <= 0.0) {
            return 1.0;  // Maximum risk if portfolio value is zero or negative
        }

        // Calculate risk components
        const double leverage_risk =
            calculate_portfolio_risk(portfolio) / max_leverage_;
        const double pnl_risk = std::abs(portfolio.get_total_unrealized_pnl()) /
                                (total_value * max_drawdown_);

        // Combine risk measures
        return std::min(1.0, std::max(leverage_risk, pnl_risk));
    } catch (const std::exception &e) {
        auto &logger = utils::Logger::get_instance();
        logger.error("Error calculating risk level: " + std::string(e.what()));
        return 1.0;  // Return maximum risk on error
    }
}

void RiskManager::update_risk_params(const Portfolio &portfolio) {
    try {
        const double current_risk = get_current_risk_level(portfolio);
        auto &logger = utils::Logger::get_instance();

        // Dynamically adjust risk parameters based on portfolio performance
        if (current_risk > 0.8) {  // High risk situation
            max_position_size_ *= 0.9;
            max_risk_per_trade_ *= 0.9;
            logger.info("Reducing risk limits due to high risk level");
        } else if (current_risk < 0.3) {  // Low risk situation
            const double max_allowed_position =
                config_.get_double("risk.maxPositionSize", 100000.0);
            const double max_allowed_risk =
                config_.get_double("risk.maxRiskPerTrade", 0.02);

            if (max_position_size_ < max_allowed_position) {
                max_position_size_ =
                    std::min(max_position_size_ * 1.1, max_allowed_position);
            }
            if (max_risk_per_trade_ < max_allowed_risk) {
                max_risk_per_trade_ =
                    std::min(max_risk_per_trade_ * 1.1, max_allowed_risk);
            }
            logger.info("Increasing risk limits due to low risk level");
        }

        logger.debug(
            "Updated risk parameters - Max position size: " +
            std::to_string(max_position_size_) +
            ", Max risk per trade: " + std::to_string(max_risk_per_trade_));
    } catch (const std::exception &e) {
        auto &logger = utils::Logger::get_instance();
        logger.error("Error updating risk parameters: " +
                     std::string(e.what()));
    }
}

double RiskManager::calculate_position_risk(const std::string &symbol,
                                            double quantity,
                                            const Portfolio &portfolio) const {
    const double position_value =
        std::abs(quantity * portfolio.get_position(symbol).last_price);
    const double portfolio_value = portfolio.get_total_value();

    return portfolio_value > 0.0 ? position_value / portfolio_value : 1.0;
}

double RiskManager::calculate_portfolio_risk(const Portfolio &portfolio) const {
    const double total_value = portfolio.get_total_value();
    if (total_value <= 0.0) {
        return std::numeric_limits<double>::max();
    }

    const auto positions = portfolio.get_positions();
    double total_exposure = 0.0;

    for (const auto &pos : positions) {
        total_exposure += std::abs(pos.get_value());
    }

    return total_exposure / total_value;
}

}  // namespace core
}  // namespace thales
