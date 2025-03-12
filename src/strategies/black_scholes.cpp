#include <thales/strategies/black_scholes.h>
#include <thales/utils/logger.h>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace thales {
namespace strategies {

BlackScholes::BlackScholes(const utils::Config& config)
    : StrategyBase("BlackScholes", config),
      min_price_discrepancy_(0.05),
      min_volatility_(0.15),
      max_volatility_(0.50),
      min_days_to_expiration_(7),
      max_days_to_expiration_(45) {
    
    description_ = "Strategy based on Black-Scholes pricing model";
}

bool BlackScholes::initialize() {
    auto& logger = utils::Logger::getInstance();
    logger.info("Initializing BlackScholes strategy");
    
    try {
        // Load strategy parameters from configuration
        if (config_.hasKey("strategies.BlackScholes.minPriceDiscrepancy")) {
            min_price_discrepancy_ = config_.getDouble("strategies.BlackScholes.minPriceDiscrepancy");
        }
        
        if (config_.hasKey("strategies.BlackScholes.minVolatility")) {
            min_volatility_ = config_.getDouble("strategies.BlackScholes.minVolatility");
        }
        
        if (config_.hasKey("strategies.BlackScholes.maxVolatility")) {
            max_volatility_ = config_.getDouble("strategies.BlackScholes.maxVolatility");
        }
        
        if (config_.hasKey("strategies.BlackScholes.minDaysToExpiration")) {
            min_days_to_expiration_ = config_.getInt("strategies.BlackScholes.minDaysToExpiration");
        }
        
        if (config_.hasKey("strategies.BlackScholes.maxDaysToExpiration")) {
            max_days_to_expiration_ = config_.getInt("strategies.BlackScholes.maxDaysToExpiration");
        }
        
        // Load symbols from configuration
        load_symbols();
        
        logger.info("BlackScholes strategy initialized successfully");
        return true;
    } catch (const std::exception& e) {
        logger.error("Exception during BlackScholes initialization: " + std::string(e.what()));
        return false;
    } catch (...) {
        logger.error("Unknown exception during BlackScholes initialization");
        return false;
    }
}

std::vector<Signal> BlackScholes::execute(
    const std::vector<data::MarketData>& market_data,
    const core::Portfolio& portfolio
) {
    auto& logger = utils::Logger::getInstance();
    logger.debug("Executing BlackScholes strategy");
    
    std::vector<Signal> signals;
    
    try {
        // Update historical volatility for each symbol
        for (const auto& symbol : symbols_) {
            historical_volatility_[symbol] = calculate_historical_volatility(market_data, symbol);
        }
        
        // Get the risk-free rate
        double risk_free_rate = calculate_risk_free_rate();
        
        // Process each market data point
        for (const auto& data : market_data) {
            // Check if the data is for an option
            const data::OptionData* option_data = dynamic_cast<const data::OptionData*>(&data);
            if (!option_data) {
                continue;
            }
            
            // Check if the option is eligible for trading
            if (!is_option_eligible(*option_data)) {
                continue;
            }
            
            // Get the underlying symbol
            const std::string& underlying_symbol = option_data->underlyingSymbol;
            
            // Get the underlying price
            double underlying_price = 0.0;
            for (const auto& d : market_data) {
                if (d.symbol == underlying_symbol) {
                    underlying_price = d.price;
                    break;
                }
            }
            
            if (underlying_price <= 0.0) {
                logger.warning("Could not find underlying price for " + underlying_symbol);
                continue;
            }
            
            // Get the historical volatility
            auto it = historical_volatility_.find(underlying_symbol);
            if (it == historical_volatility_.end()) {
                logger.warning("Could not find historical volatility for " + underlying_symbol);
                continue;
            }
            double volatility = it->second;
            
            // Calculate time to expiration in years
            double time_to_expiration = calculate_time_to_expiration(option_data->expirationDate);
            
            // Calculate the theoretical price using Black-Scholes
            double theoretical_price = 0.0;
            if (option_data->isCall) {
                theoretical_price = models::BlackScholes::callPrice(
                    underlying_price,
                    option_data->strikePrice,
                    risk_free_rate,
                    volatility,
                    time_to_expiration
                );
            } else {
                theoretical_price = models::BlackScholes::putPrice(
                    underlying_price,
                    option_data->strikePrice,
                    risk_free_rate,
                    volatility,
                    time_to_expiration
                );
            }
            
            // Calculate the price discrepancy
            double market_price = option_data->price;
            double price_discrepancy = (market_price - theoretical_price) / theoretical_price;
            
            // Generate a signal if the discrepancy is significant
            if (std::abs(price_discrepancy) >= min_price_discrepancy_) {
                Signal signal = generate_signal(*option_data, theoretical_price);
                signals.push_back(signal);
                
                logger.info("Generated signal for " + option_data->symbol + 
                           ": Market price = " + std::to_string(market_price) + 
                           ", Theoretical price = " + std::to_string(theoretical_price) + 
                           ", Discrepancy = " + std::to_string(price_discrepancy * 100.0) + "%");
            }
        }
        
        logger.debug("BlackScholes strategy generated " + std::to_string(signals.size()) + " signals");
        return signals;
    } catch (const std::exception& e) {
        logger.error("Exception during BlackScholes execution: " + std::string(e.what()));
        return signals;
    } catch (...) {
        logger.error("Unknown exception during BlackScholes execution");
        return signals;
    }
}

std::vector<std::string> BlackScholes::get_symbols() const {
    return symbols_;
}

double BlackScholes::calculate_historical_volatility(
    const std::vector<data::MarketData>& market_data,
    const std::string& symbol
) {
    // This is a simplified implementation of historical volatility calculation
    // In a real implementation, you would use a more sophisticated method
    
    std::vector<double> returns;
    double prev_close = 0.0;
    
    // Calculate daily returns
    for (const auto& data : market_data) {
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
    double mean = sum / returns.size();
    
    double sum_squared_diff = 0.0;
    for (double ret : returns) {
        double diff = ret - mean;
        sum_squared_diff += diff * diff;
    }
    
    double variance = sum_squared_diff / (returns.size() - 1);
    double std_dev = std::sqrt(variance);
    
    // Annualize the volatility (assuming daily returns)
    double annualized_vol = std_dev * std::sqrt(252.0);
    
    // Clamp the volatility to the allowed range
    return std::clamp(annualized_vol, min_volatility_, max_volatility_);
}

double BlackScholes::calculate_risk_free_rate() const {
    // In a real implementation, you would get the risk-free rate from a data source
    // For simplicity, we'll use a fixed value
    return 0.02; // 2% risk-free rate
}

double BlackScholes::calculate_time_to_expiration(const std::string& expiration_date) const {
    // Parse the expiration date (format: YYYY-MM-DD)
    std::tm expiration_tm = {};
    std::istringstream ss(expiration_date);
    ss >> std::get_time(&expiration_tm, "%Y-%m-%d");
    
    // Get the current date
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm* now_tm = std::localtime(&now_time);
    
    // Convert to time_t
    std::time_t expiration_time = std::mktime(&expiration_tm);
    std::time_t current_time = std::mktime(now_tm);
    
    // Calculate the difference in seconds
    double diff_seconds = std::difftime(expiration_time, current_time);
    
    // Convert to years
    return diff_seconds / (365.25 * 24 * 60 * 60);
}

bool BlackScholes::is_option_eligible(const data::OptionData& option) const {
    // Parse the expiration date (format: YYYY-MM-DD)
    std::tm expiration_tm = {};
    std::istringstream ss(option.expirationDate);
    ss >> std::get_time(&expiration_tm, "%Y-%m-%d");
    
    // Get the current date
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm* now_tm = std::localtime(&now_time);
    
    // Convert to time_t
    std::time_t expiration_time = std::mktime(&expiration_tm);
    std::time_t current_time = std::mktime(now_tm);
    
    // Calculate the difference in days
    double diff_seconds = std::difftime(expiration_time, current_time);
    int diff_days = static_cast<int>(diff_seconds / (24 * 60 * 60));
    
    // Check if the option meets the criteria
    return diff_days >= min_days_to_expiration_ && diff_days <= max_days_to_expiration_;
}

Signal BlackScholes::generate_signal(const data::OptionData& option, double theoretical_price) {
    Signal::Type signal_type;
    double strength;
    
    // Determine the signal type and strength based on the price discrepancy
    double market_price = option.price;
    double price_discrepancy = (market_price - theoretical_price) / theoretical_price;
    
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
    std::tm* now_tm = std::localtime(&now_time);
    std::stringstream ss;
    ss << std::put_time(now_tm, "%Y-%m-%d %H:%M:%S");
    std::string timestamp = ss.str();
    
    // Create and return the signal
    return Signal(
        option.symbol,
        signal_type,
        strength,
        theoretical_price,
        0.0, // No stop loss for now
        timestamp,
        "BlackScholes"
    );
}

} // namespace strategies
} // namespace thales
