#include <thales/strategies/black_scholes_arbitrage.h>
#include <thales/utils/logger.h>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace thales {
namespace strategies {

BlackScholesArbitrage::BlackScholesArbitrage(const utils::Config& config)
    : StrategyBase("BlackScholesArbitrage", config),
      minPriceDiscrepancy_(0.05),
      minVolatility_(0.15),
      maxVolatility_(0.50),
      minDaysToExpiration_(7),
      maxDaysToExpiration_(45) {
    
    description_ = "Strategy that looks for arbitrage opportunities based on Black-Scholes pricing";
}

bool BlackScholesArbitrage::initialize() {
    auto& logger = utils::Logger::getInstance();
    logger.info("Initializing BlackScholesArbitrage strategy");
    
    try {
        // Load strategy parameters from configuration
        if (config_.hasKey("strategies.BlackScholesArbitrage.minPriceDiscrepancy")) {
            minPriceDiscrepancy_ = config_.getDouble("strategies.BlackScholesArbitrage.minPriceDiscrepancy");
        }
        
        if (config_.hasKey("strategies.BlackScholesArbitrage.minVolatility")) {
            minVolatility_ = config_.getDouble("strategies.BlackScholesArbitrage.minVolatility");
        }
        
        if (config_.hasKey("strategies.BlackScholesArbitrage.maxVolatility")) {
            maxVolatility_ = config_.getDouble("strategies.BlackScholesArbitrage.maxVolatility");
        }
        
        if (config_.hasKey("strategies.BlackScholesArbitrage.minDaysToExpiration")) {
            minDaysToExpiration_ = config_.getInt("strategies.BlackScholesArbitrage.minDaysToExpiration");
        }
        
        if (config_.hasKey("strategies.BlackScholesArbitrage.maxDaysToExpiration")) {
            maxDaysToExpiration_ = config_.getInt("strategies.BlackScholesArbitrage.maxDaysToExpiration");
        }
        
        // Load symbols from configuration
        loadSymbols();
        
        logger.info("BlackScholesArbitrage strategy initialized successfully");
        return true;
    } catch (const std::exception& e) {
        logger.error("Exception during BlackScholesArbitrage initialization: " + std::string(e.what()));
        return false;
    } catch (...) {
        logger.error("Unknown exception during BlackScholesArbitrage initialization");
        return false;
    }
}

std::vector<Signal> BlackScholesArbitrage::execute(
    const std::vector<data::MarketData>& marketData,
    const core::Portfolio& portfolio
) {
    auto& logger = utils::Logger::getInstance();
    logger.debug("Executing BlackScholesArbitrage strategy");
    
    std::vector<Signal> signals;
    
    try {
        // Update historical volatility for each symbol
        for (const auto& symbol : symbols_) {
            historicalVolatility_[symbol] = calculateHistoricalVolatility(marketData, symbol);
        }
        
        // Get the risk-free rate
        double riskFreeRate = calculateRiskFreeRate();
        
        // Process each market data point
        for (const auto& data : marketData) {
            // Check if the data is for an option
            const data::OptionData* optionData = dynamic_cast<const data::OptionData*>(&data);
            if (!optionData) {
                continue;
            }
            
            // Check if the option is eligible for trading
            if (!isOptionEligible(*optionData)) {
                continue;
            }
            
            // Get the underlying symbol
            const std::string& underlyingSymbol = optionData->underlyingSymbol;
            
            // Get the underlying price
            double underlyingPrice = 0.0;
            for (const auto& d : marketData) {
                if (d.symbol == underlyingSymbol) {
                    underlyingPrice = d.price;
                    break;
                }
            }
            
            if (underlyingPrice <= 0.0) {
                logger.warning("Could not find underlying price for " + underlyingSymbol);
                continue;
            }
            
            // Get the historical volatility
            auto it = historicalVolatility_.find(underlyingSymbol);
            if (it == historicalVolatility_.end()) {
                logger.warning("Could not find historical volatility for " + underlyingSymbol);
                continue;
            }
            double volatility = it->second;
            
            // Calculate time to expiration in years
            double timeToExpiration = calculateTimeToExpiration(optionData->expirationDate);
            
            // Calculate the theoretical price using Black-Scholes
            double theoreticalPrice = 0.0;
            if (optionData->isCall) {
                theoreticalPrice = models::BlackScholes::callPrice(
                    underlyingPrice,
                    optionData->strikePrice,
                    riskFreeRate,
                    volatility,
                    timeToExpiration
                );
            } else {
                theoreticalPrice = models::BlackScholes::putPrice(
                    underlyingPrice,
                    optionData->strikePrice,
                    riskFreeRate,
                    volatility,
                    timeToExpiration
                );
            }
            
            // Calculate the price discrepancy
            double marketPrice = optionData->price;
            double priceDiscrepancy = (marketPrice - theoreticalPrice) / theoreticalPrice;
            
            // Generate a signal if the discrepancy is significant
            if (std::abs(priceDiscrepancy) >= minPriceDiscrepancy_) {
                Signal signal = generateSignal(*optionData, theoreticalPrice);
                signals.push_back(signal);
                
                logger.info("Generated signal for " + optionData->symbol + 
                           ": Market price = " + std::to_string(marketPrice) + 
                           ", Theoretical price = " + std::to_string(theoreticalPrice) + 
                           ", Discrepancy = " + std::to_string(priceDiscrepancy * 100.0) + "%");
            }
        }
        
        logger.debug("BlackScholesArbitrage strategy generated " + std::to_string(signals.size()) + " signals");
        return signals;
    } catch (const std::exception& e) {
        logger.error("Exception during BlackScholesArbitrage execution: " + std::string(e.what()));
        return signals;
    } catch (...) {
        logger.error("Unknown exception during BlackScholesArbitrage execution");
        return signals;
    }
}

std::vector<std::string> BlackScholesArbitrage::getSymbols() const {
    return symbols_;
}

double BlackScholesArbitrage::calculateHistoricalVolatility(
    const std::vector<data::MarketData>& marketData,
    const std::string& symbol
) {
    // This is a simplified implementation of historical volatility calculation
    // In a real implementation, you would use a more sophisticated method
    
    std::vector<double> returns;
    double prevClose = 0.0;
    
    // Calculate daily returns
    for (const auto& data : marketData) {
        if (data.symbol == symbol) {
            if (prevClose > 0.0) {
                double ret = std::log(data.close / prevClose);
                returns.push_back(ret);
            }
            prevClose = data.close;
        }
    }
    
    if (returns.empty()) {
        return minVolatility_; // Default to minimum volatility if no data
    }
    
    // Calculate standard deviation of returns
    double sum = 0.0;
    for (double ret : returns) {
        sum += ret;
    }
    double mean = sum / returns.size();
    
    double sumSquaredDiff = 0.0;
    for (double ret : returns) {
        double diff = ret - mean;
        sumSquaredDiff += diff * diff;
    }
    
    double variance = sumSquaredDiff / (returns.size() - 1);
    double stdDev = std::sqrt(variance);
    
    // Annualize the volatility (assuming daily returns)
    double annualizedVol = stdDev * std::sqrt(252.0);
    
    // Clamp the volatility to the allowed range
    return std::clamp(annualizedVol, minVolatility_, maxVolatility_);
}

double BlackScholesArbitrage::calculateRiskFreeRate() const {
    // In a real implementation, you would get the risk-free rate from a data source
    // For simplicity, we'll use a fixed value
    return 0.02; // 2% risk-free rate
}

double BlackScholesArbitrage::calculateTimeToExpiration(const std::string& expirationDate) const {
    // Parse the expiration date (format: YYYY-MM-DD)
    std::tm expirationTm = {};
    std::istringstream ss(expirationDate);
    ss >> std::get_time(&expirationTm, "%Y-%m-%d");
    
    // Get the current date
    auto now = std::chrono::system_clock::now();
    std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
    std::tm* nowTm = std::localtime(&nowTime);
    
    // Convert to time_t
    std::time_t expirationTime = std::mktime(&expirationTm);
    std::time_t currentTime = std::mktime(nowTm);
    
    // Calculate the difference in seconds
    double diffSeconds = std::difftime(expirationTime, currentTime);
    
    // Convert to years
    return diffSeconds / (365.25 * 24 * 60 * 60);
}

bool BlackScholesArbitrage::isOptionEligible(const data::OptionData& option) const {
    // Parse the expiration date (format: YYYY-MM-DD)
    std::tm expirationTm = {};
    std::istringstream ss(option.expirationDate);
    ss >> std::get_time(&expirationTm, "%Y-%m-%d");
    
    // Get the current date
    auto now = std::chrono::system_clock::now();
    std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
    std::tm* nowTm = std::localtime(&nowTime);
    
    // Convert to time_t
    std::time_t expirationTime = std::mktime(&expirationTm);
    std::time_t currentTime = std::mktime(nowTm);
    
    // Calculate the difference in days
    double diffSeconds = std::difftime(expirationTime, currentTime);
    int diffDays = static_cast<int>(diffSeconds / (24 * 60 * 60));
    
    // Check if the option meets the criteria
    return diffDays >= minDaysToExpiration_ && diffDays <= maxDaysToExpiration_;
}

Signal BlackScholesArbitrage::generateSignal(const data::OptionData& option, double theoreticalPrice) {
    Signal::Type signalType;
    double strength;
    
    // Determine the signal type and strength based on the price discrepancy
    double marketPrice = option.price;
    double priceDiscrepancy = (marketPrice - theoreticalPrice) / theoreticalPrice;
    
    if (priceDiscrepancy > 0) {
        // Market price is higher than theoretical price, so sell
        signalType = Signal::Type::SELL;
    } else {
        // Market price is lower than theoretical price, so buy
        signalType = Signal::Type::BUY;
    }
    
    // Calculate signal strength based on the magnitude of the discrepancy
    strength = std::min(1.0, std::abs(priceDiscrepancy) / 0.2);
    
    // Get the current timestamp
    auto now = std::chrono::system_clock::now();
    std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
    std::tm* nowTm = std::localtime(&nowTime);
    std::stringstream ss;
    ss << std::put_time(nowTm, "%Y-%m-%d %H:%M:%S");
    std::string timestamp = ss.str();
    
    // Create and return the signal
    return Signal(
        option.symbol,
        signalType,
        strength,
        theoreticalPrice,
        0.0, // No stop loss for now
        timestamp,
        "BlackScholesArbitrage"
    );
}

} // namespace strategies
} // namespace thales
