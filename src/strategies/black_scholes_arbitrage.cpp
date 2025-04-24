// SPDX-License-Identifier: MIT

#include <algorithm>
#include <chrono>  // For std::chrono
#include <cmath>   // For std::log, std::sqrt
#include <ctime>   // For std::mktime, std::localtime
#include <iomanip> // For std::get_time, std::put_time
#include <sstream> // For std::stringstream, std::istringstream
#include <thales/strategies/black_scholes_arbitrage.hpp>
#include <thales/utils/logger.hpp>

namespace thales {
namespace strategies {

BlackScholesArbitrage::BlackScholesArbitrage(const utils::Config &config)
    : StrategyBase("BlackScholesArbitrage", config),
      min_price_discrepancy_(0.05), min_volatility_(0.15),
      max_volatility_(0.50), min_days_to_expiration_(7),
      max_days_to_expiration_(45) {
    description_ = "Strategy based on Black-Scholes pricing model";
}

bool BlackScholesArbitrage::initialize() {
    auto &logger = utils::Logger::get_instance();
    logger.info("Initializing BlackScholesArbitrage strategy");

    // Load strategy parameters from configuration
    if (config_.has_key(
            "strategies.BlackScholesArbitrage.minPriceDiscrepancy")) {
        min_price_discrepancy_ = config_.get_double(
            "strategies.BlackScholesArbitrage.minPriceDiscrepancy");
    }

    if (config_.has_key("strategies.BlackScholesArbitrage.minVolatility")) {
        min_volatility_ = config_.get_double(
            "strategies.BlackScholesArbitrage.minVolatility");
    }

    if (config_.has_key("strategies.BlackScholesArbitrage.maxVolatility")) {
        max_volatility_ = config_.get_double(
            "strategies.BlackScholesArbitrage.maxVolatility");
    }

    if (config_.has_key(
            "strategies.BlackScholesArbitrage.minDaysToExpiration")) {
        min_days_to_expiration_ = config_.get_int(
            "strategies.BlackScholesArbitrage.minDaysToExpiration");
    }

    if (config_.has_key(
            "strategies.BlackScholesArbitrage.maxDaysToExpiration")) {
        max_days_to_expiration_ = config_.get_int(
            "strategies.BlackScholesArbitrage.maxDaysToExpiration");
    }

    // Load symbols from configuration
    load_symbols();

    if (symbols_.empty()) {
        logger.error(
            "Failed to load symbols for BlackScholesArbitrage strategy");
        return false;
    }

    logger.info("BlackScholesArbitrage strategy initialized successfully");
    return true;
}

std::vector<Signal> BlackScholesArbitrage::execute(
    const std::vector<data::MarketData> &market_data,
    [[maybe_unused]] const core::Portfolio &portfolio) {
    auto &logger = utils::Logger::get_instance();
    logger.debug("Executing BlackScholesArbitrage strategy");

    std::vector<Signal> signals;

    // Update historical volatility for each symbol
    for (const auto &symbol : symbols_) {
        historical_volatility_[symbol] =
            calculate_historical_volatility(market_data, symbol);
    }

    // Get the risk-free rate
    double risk_free_rate = calculate_risk_free_rate();

    // Process each market data point
    for (const auto &data : market_data) {
        // Check if the data is for an option
        const data::OptionData *option_data =
            dynamic_cast<const data::OptionData *>(&data);
        if (!option_data) {
            continue;
        }

        // Check if the option is eligible for trading
        if (!is_option_eligible(*option_data)) {
            continue;
        }

        // Get the underlying symbol
        const std::string &underlying_symbol = option_data->underlying_symbol;

        // Get the underlying price
        double underlying_price = 0.0;
        for (const auto &d : market_data) {
            if (d.symbol == underlying_symbol) {
                underlying_price = d.price;
                break;
            }
        }

        if (underlying_price <= 0.0) {
            logger.warning("Could not find underlying price for " +
                           underlying_symbol);
            continue;
        }

        // Get the historical volatility
        auto it = historical_volatility_.find(underlying_symbol);
        if (it == historical_volatility_.end()) {
            logger.warning("Could not find historical volatility for " +
                           underlying_symbol);
            continue;
        }
        double volatility = it->second;

        // Calculate time to expiration in years
        double time_to_expiration =
            calculate_time_to_expiration(option_data->expiration_date);

        // Calculate the theoretical price using Black-Scholes
        double theoretical_price = 0.0;
        if (option_data->is_call) {
            theoretical_price = models::BlackScholesModel::call_price(
                underlying_price, option_data->strike_price, risk_free_rate,
                volatility, time_to_expiration);
        } else {
            theoretical_price = models::BlackScholesModel::put_price(
                underlying_price, option_data->strike_price, risk_free_rate,
                volatility, time_to_expiration);
        }

        // Calculate the price discrepancy
        double market_price = option_data->price;
        double price_discrepancy =
            (market_price - theoretical_price) / theoretical_price;

        // Generate a signal if the discrepancy is significant
        if (std::abs(price_discrepancy) >= min_price_discrepancy_) {
            Signal signal = generate_signal(*option_data, theoretical_price);
            signals.push_back(signal);

            logger.info(
                "Generated signal for " + option_data->symbol +
                ": Market price = " + std::to_string(market_price) +
                ", Theoretical price = " + std::to_string(theoretical_price) +
                ", Discrepancy = " + std::to_string(price_discrepancy * 100.0) +
                "%");
        }
    }

    logger.debug("BlackScholesArbitrage strategy generated " +
                 std::to_string(signals.size()) + " signals");
    return signals;
}

std::vector<std::string> BlackScholesArbitrage::get_symbols() const {
    return symbols_;
}

double BlackScholesArbitrage::calculate_historical_volatility(
    const std::vector<data::MarketData> &market_data,
    const std::string &symbol) {
    // This is a simplified implementation of historical volatility calculation
    // In a real implementation, you would use a more sophisticated method

    std::vector<double> returns;
    double prev_close = 0.0;

    // Calculate daily returns
    for (const auto &data : market_data) {
        if (data.symbol == symbol) {
            if (prev_close > 0.0) {
                double ret = std::log(data.close / prev_close);
                returns.push_back(ret);
            }
            prev_close = data.close;
        }
    }

    if (returns.empty()) {
        return min_volatility_; // Default to minimum volatility if no data
    }

    // Calculate standard deviation of returns
    double sum = 0.0;
    for (double ret : returns) {
        sum += ret;
    }
    double mean = sum / static_cast<double>(returns.size());

    double sum_squared_diff = 0.0;
    for (double ret : returns) {
        double diff = ret - mean;
        sum_squared_diff += diff * diff;
    }

    double variance =
        sum_squared_diff / static_cast<double>(returns.size() - 1);
    double std_dev = std::sqrt(variance);

    // Annualize the volatility (assuming daily returns)
    double annualized_vol = std_dev * std::sqrt(252.0);

    // Clamp the volatility to the allowed range
    return std::clamp(annualized_vol, min_volatility_, max_volatility_);
}

double BlackScholesArbitrage::calculate_risk_free_rate() const {
    // In a real implementation, you would get the risk-free rate from a data
    // source For simplicity, we'll use a fixed value
    return 0.02; // 2% risk-free rate
}

double BlackScholesArbitrage::calculate_time_to_expiration(
    const std::string &expiration_date) const {
    // Parse the expiration date (format: YYYY-MM-DD)
    std::tm expiration_tm = {};
    std::istringstream ss(expiration_date);
    ss >> std::get_time(&expiration_tm, "%Y-%m-%d");

    // Get the current date
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm *now_tm = std::localtime(&now_time);

    // Convert to time_t
    std::time_t expiration_time = std::mktime(&expiration_tm);
    std::time_t current_time = std::mktime(now_tm);

    // Calculate the difference in seconds
    double diff_seconds = std::difftime(expiration_time, current_time);

    // Convert to years
    return diff_seconds / (365.25 * 24 * 60 * 60);
}

bool BlackScholesArbitrage::is_option_eligible(
    const data::OptionData &option) const {
    // Parse the expiration date (format: YYYY-MM-DD)
    std::tm expiration_tm = {};
    std::istringstream ss(option.expiration_date);
    ss >> std::get_time(&expiration_tm, "%Y-%m-%d");

    // Get the current date
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm *now_tm = std::localtime(&now_time);

    // Convert to time_t
    std::time_t expiration_time = std::mktime(&expiration_tm);
    std::time_t current_time = std::mktime(now_tm);

    // Calculate the difference in days
    double diff_seconds = std::difftime(expiration_time, current_time);
    int diff_days = static_cast<int>(diff_seconds / (24 * 60 * 60));

    // Check if the option meets the criteria
    return diff_days >= min_days_to_expiration_ &&
           diff_days <= max_days_to_expiration_;
}

Signal BlackScholesArbitrage::generate_signal(const data::OptionData &option,
                                              double theoretical_price) {
    Signal::Type signal_type;
    double strength;

    // Determine the signal type and strength based on the price discrepancy
    double market_price = option.price;
    double price_discrepancy =
        (market_price - theoretical_price) / theoretical_price;

    if (price_discrepancy > 0) {
        // Market price is higher than theoretical price, so sell
        signal_type = Signal::Type::SELL;
    } else {
        // Market price is lower than theoretical price, so buy
        signal_type = Signal::Type::BUY;
    }

    // Calculate signal strength based on the magnitude of the discrepancy
    strength = std::min(1.0, std::abs(price_discrepancy) / 0.2);

    // Get the current timestamp
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm *now_tm = std::localtime(&now_time);
    std::stringstream ss;
    ss << std::put_time(now_tm, "%Y-%m-%d %H:%M:%S");
    std::string timestamp = ss.str();

    // Create SignalParams and return the signal
    Signal::SignalParams params;
    params.symbol = option.symbol;
    params.type = signal_type;
    params.strength = strength;
    params.target_price = theoretical_price;
    params.stop_loss = 0.0; // No stop loss for now
    params.timestamp = timestamp;
    params.strategy_name = "BlackScholesArbitrage";
    return Signal(params);
}

} // namespace strategies
} // namespace thales
